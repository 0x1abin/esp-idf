/* SPDX-License-Identifier: Apache-2.0 */
#pragma once

#include <stdint.h>

void sf32lb57_board_init(void);
uint32_t sf32lb57_cpu_hz(void);
uint16_t sf32lb57_chip_revision(void);
uint32_t sf32lb57_nor_size(void);
int sf32lb57_nor_read(void *buffer, uint32_t offset, uint32_t size);
void sf32lb57_board_tick(void);
int sf32lb57_console_write(const void *buffer, uint16_t size);
void sf32lb57_board_restart(void) __attribute__((noreturn));
int sf32lb57_probe_sdio(uint16_t *vendor, uint16_t *device);
