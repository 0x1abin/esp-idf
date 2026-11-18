/* SPDX-License-Identifier: Apache-2.0 */

#include <stdint.h>
#include "rtconfig.h"
#include "bf0_hal.h"
#include "bf0_hal_sdmmc.h"
#include "sf32lb57_board.h"

#define AW9523_ADDRESS 0x59
#define AW9523_OUTPUT1 0x03
#define AW9523_CONFIG1 0x05
#define AW9523_ID      0x10
#define AW9523_MODE1   0x13
#define WIFI_POWER     (1U << 5)
#define WIFI_ENABLE    (1U << 4)

#define SDIO_OCR_READY (UINT32_C(1) << 31)
#define SDIO_OCR_3V3   UINT32_C(0x00300000)
#define SDIO_R5_ERRORS UINT32_C(0xcb00)
#define SDIO_CIS_ADDR  0x09
#define SDIO_CIS_MANFID 0x20

static HAL_StatusTypeDef aw_read(I2C_HandleTypeDef *bus, uint8_t reg, uint8_t *value)
{
    return HAL_I2C_Mem_Read(bus, AW9523_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, value, 1, 50);
}

static HAL_StatusTypeDef aw_write(I2C_HandleTypeDef *bus, uint8_t reg, uint8_t value)
{
    return HAL_I2C_Mem_Write(bus, AW9523_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 50);
}

static HAL_StatusTypeDef wifi_power_on(I2C_HandleTypeDef *bus, uint8_t *output)
{
    HAL_RCC_EnableModule(RCC_MOD_I2C1);
    bus->Instance = hwp_i2c1;
    bus->Mode = HAL_I2C_MODE_MASTER;
    bus->Init.ClockSpeed = 100000;
    bus->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    bus->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    if (HAL_I2C_Init(bus) != HAL_OK) {
        return HAL_ERROR;
    }
    __HAL_I2C_ENABLE(bus);

    uint8_t id, mode, config;
    const uint8_t mask = WIFI_POWER | WIFI_ENABLE;
    if (aw_read(bus, AW9523_ID, &id) != HAL_OK || id != 0x23 ||
        aw_read(bus, AW9523_MODE1, &mode) != HAL_OK ||
        aw_read(bus, AW9523_CONFIG1, &config) != HAL_OK ||
        aw_read(bus, AW9523_OUTPUT1, output) != HAL_OK) {
        return HAL_ERROR;
    }

    *output &= (uint8_t)~mask;
    if (aw_write(bus, AW9523_OUTPUT1, *output) != HAL_OK ||
        aw_write(bus, AW9523_MODE1, mode | mask) != HAL_OK ||
        aw_write(bus, AW9523_CONFIG1, config & (uint8_t)~mask) != HAL_OK) {
        return HAL_ERROR;
    }
    HAL_Delay(10);
    if (aw_write(bus, AW9523_OUTPUT1, *output | WIFI_POWER) != HAL_OK ||
        aw_write(bus, AW9523_OUTPUT1, *output | mask) != HAL_OK) {
        aw_write(bus, AW9523_OUTPUT1, *output);
        return HAL_ERROR;
    }
    HAL_Delay(200);
    return HAL_OK;
}

static int sdio_command(uint8_t cmd, uint32_t argument, uint32_t *response)
{
    const uint32_t complete = response ? SD_SR_CMD_DONE : SD_SR_CMD_SENT;
    const uint32_t start = HAL_GetTick();
    HAL_SDMMC_SET_CMD(hwp_sdmmc2, cmd, response != NULL, argument);
    do {
        uint32_t status = HAL_SDMMC_GET_STA(hwp_sdmmc2);
        if ((status & SD_SR_CMD_TIMEOUT) || (cmd != 5 && (status & SD_SR_CMD_RSP_CRC))) {
            return -1;
        }
        /* R4 (CMD5) carries no CRC. */
        if ((status & complete) || (cmd == 5 && (status & SD_SR_CMD_RSP_CRC))) {
            if (response) {
                *response = hwp_sdmmc2->RAR1;
            }
            return 0;
        }
    } while (HAL_GetTick() - start < 50);
    return -1;
}

static int sdio_read_byte(uint32_t address, uint8_t *value)
{
    uint32_t result;
    if (address > 0x1ffff || sdio_command(52, address << 9, &result) || (result & SDIO_R5_ERRORS)) {
        return -1;
    }
    *value = result & 0xff;
    return 0;
}

static int sdio_read_manufacturer(uint16_t *vendor, uint16_t *device)
{
    uint32_t cis = 0;
    for (unsigned i = 0; i < 3; ++i) {
        uint8_t byte;
        if (sdio_read_byte(SDIO_CIS_ADDR + i, &byte)) {
            return -1;
        }
        cis |= (uint32_t)byte << (i * 8);
    }
    if (cis < 0x1000 || cis > 0x1ffff) {
        return -1;
    }

    /* Bound malformed or empty CIS reads. */
    for (unsigned scanned = 0; scanned < 64; ++scanned) {
        uint8_t type, length;
        if (sdio_read_byte(cis++, &type) || type == 0xff) {
            break;
        }
        if (type == 0x00) {
            continue;
        }
        if (sdio_read_byte(cis++, &length) || length == 0xff) {
            break;
        }
        if (type == SDIO_CIS_MANFID && length >= 4) {
            uint8_t id[4];
            for (unsigned i = 0; i < sizeof(id); ++i) {
                if (sdio_read_byte(cis + i, &id[i])) {
                    return -1;
                }
            }
            *vendor = (uint16_t)((id[1] << 8) | id[0]);
            *device = (uint16_t)((id[3] << 8) | id[2]);
            return 0;
        }
        cis += length;
    }
    return -1;
}

static int sdio_enumerate(uint16_t *vendor, uint16_t *device)
{
    HAL_PIN_Set(PAD_PA02, SD2_CLK, PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_PA03, SD2_CMD, PIN_PULLUP, 1);
    HAL_PIN_Set(PAD_PA04, SD2_DIO0, PIN_PULLUP, 1);
    HAL_PIN_Set(PAD_PA05, SD2_DIO1, PIN_PULLUP, 1);
    HAL_PIN_Set(PAD_PA00, SD2_DIO2, PIN_PULLUP, 1);
    HAL_PIN_Set(PAD_PA01, SD2_DIO3, PIN_PULLUP, 1);

    HAL_RCC_EnableModule(RCC_MOD_SDMMC2);
    HAL_SDMMC_INIT(hwp_sdmmc2);
    HAL_SDMMC_SET_WIRE(hwp_sdmmc2, HAL_SDMMC_WIRE_SINGLE);
    HAL_SDMMC_POWER_SET(hwp_sdmmc2, 3);
    const uint32_t hclk = HAL_RCC_GetHCLKFreq(CORE_ID_HCPU);
    HAL_SDMMC_CLK_SET(hwp_sdmmc2, (hclk + 399999) / 400000, 1);
    HAL_Delay(2);

    uint32_t response;
    if (sdio_command(0, 0, NULL) || sdio_command(5, 0, &response)) {
        return -2;
    }
    uint32_t voltage = response & SDIO_OCR_3V3;
    if (!voltage) {
        return -3;
    }

    uint32_t start = HAL_GetTick();
    do {
        if (sdio_command(5, voltage, &response)) {
            return -2;
        }
        if (response & SDIO_OCR_READY) {
            uint32_t rca;
            if (sdio_command(3, 0, &rca) || !(rca >> 16) || sdio_command(7, rca & 0xffff0000, &response)) {
                return -4;
            }
            return sdio_read_manufacturer(vendor, device) ? -5 : 0;
        }
        HAL_Delay(10);
    } while (HAL_GetTick() - start < 1000);
    return -3;
}

int sf32lb57_probe_sdio(uint16_t *vendor, uint16_t *device)
{
    I2C_HandleTypeDef bus = {0};
    uint8_t output;
    if (wifi_power_on(&bus, &output) != HAL_OK) {
        HAL_I2C_DeInit(&bus);
        return -1;
    }

    int result = sdio_enumerate(vendor, device);
    HAL_SDMMC_CLK_SET(hwp_sdmmc2, 2, 0);
    HAL_SDMMC_POWER_SET(hwp_sdmmc2, 0);
    aw_write(&bus, AW9523_OUTPUT1, output | WIFI_POWER);
    if (aw_write(&bus, AW9523_OUTPUT1, output) != HAL_OK && result == 0) {
        result = -6;
    }
    HAL_I2C_DeInit(&bus);
    return result;
}
