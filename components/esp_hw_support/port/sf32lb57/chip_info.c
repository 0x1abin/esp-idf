/* SPDX-License-Identifier: Apache-2.0 */

#include "esp_chip_info.h"
#include "sf32lb57_board.h"
#include "soc/soc_caps.h"

void esp_chip_info(esp_chip_info_t *out_info)
{
    *out_info = (esp_chip_info_t) {
        .model = CHIP_SF32LB57,
        .revision = sf32lb57_chip_revision(),
        .cores = SOC_CPU_CORES_NUM,
    };
}
