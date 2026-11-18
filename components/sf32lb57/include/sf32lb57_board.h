/* SPDX-License-Identifier: Apache-2.0 */
#pragma once

#include <stdint.h>

void sf32lb57_board_init(void);
uint32_t sf32lb57_cpu_hz(void);
void sf32lb57_board_tick(void);
int sf32lb57_console_write(const void *buffer, uint16_t size);
void sf32lb57_board_restart(void);
int sf32lb57_probe_sdio(uint16_t *vendor, uint16_t *device);
