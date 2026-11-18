/* SPDX-License-Identifier: Apache-2.0 */

#include "rtconfig.h"
#include "bf0_hal.h"
#include "drv_io.h"
#include "sf32lb57_board.h"

static UART_HandleTypeDef console;

void sf32lb57_board_init(void)
{
    if (HAL_Init() != HAL_OK) {
        HAL_ASSERT(0);
    }
    console.Instance = hwp_usart1;
    console.Init.BaudRate = 1000000;
    console.Init.WordLength = UART_WORDLENGTH_8B;
    console.Init.StopBits = UART_STOPBITS_1;
    console.Init.Parity = UART_PARITY_NONE;
    console.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    console.Init.Mode = UART_MODE_TX_RX;
    console.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&console) != HAL_OK) {
        HAL_ASSERT(0);
    }
}

uint32_t sf32lb57_cpu_hz(void)
{
    return HAL_RCC_GetHCLKFreq(CORE_ID_HCPU);
}

uint16_t sf32lb57_chip_revision(void)
{
    return __HAL_SYSCFG_GET_REVID();
}

uint32_t sf32lb57_nor_size(void)
{
    const FLASH_HandleTypeDef *flash = BSP_Flash_get_handle(MPI3_MEM_BASE);
    return flash && !flash->isNand ? flash->size : 0;
}

int sf32lb57_nor_read(void *buffer, uint32_t offset, uint32_t size)
{
    return BSP_Nor_read(MPI3_MEM_BASE + offset, buffer, (int)size) == (int)size;
}

void sf32lb57_board_tick(void)
{
    HAL_IncTick();
}

int sf32lb57_console_write(const void *buffer, uint16_t size)
{
    return HAL_UART_Transmit(&console, (uint8_t *)buffer, size, 100) == HAL_OK ? size : -1;
}

void sf32lb57_board_restart(void)
{
    NVIC_SystemReset();
}
