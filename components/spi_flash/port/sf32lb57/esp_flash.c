/* SPDX-License-Identifier: Apache-2.0 */

#include "esp_flash.h"
#include "esp_flash_chips/esp_flash_types.h"
#include "sf32lb57_board.h"

static esp_flash_t default_chip;
esp_flash_t *esp_flash_default_chip = &default_chip;

esp_err_t esp_flash_get_size(esp_flash_t *chip, uint32_t *out_size)
{
    if ((chip != NULL && chip != &default_chip) || out_size == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    uint32_t size = sf32lb57_nor_size();
    if (size == 0) {
        return ESP_ERR_FLASH_NOT_INITIALISED;
    }
    *out_size = size;
    return ESP_OK;
}

esp_err_t esp_flash_get_physical_size(esp_flash_t *chip, uint32_t *flash_size)
{
    return esp_flash_get_size(chip, flash_size);
}

esp_err_t esp_flash_read(esp_flash_t *chip, void *buffer, uint32_t address, uint32_t length)
{
    uint32_t size;
    esp_err_t err = esp_flash_get_size(chip, &size);
    if (err != ESP_OK) {
        return err;
    }
    if (address > size || length > size - address || (length != 0 && buffer == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (length == 0) {
        return ESP_OK;
    }
    return sf32lb57_nor_read(buffer, address, length) ? ESP_OK : ESP_FAIL;
}
