// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* lcd_handle_t;

lcd_handle_t* lcd_create_obj(uint8_t spi_host, uint8_t init_bus);

// uint16_t lcd_color565(lcd_handle_t* lcd_obj, uint8_t r, uint8_t g, uint8_t b);
void lcd_drawPixel(lcd_handle_t* lcd_obj, int16_t x, int16_t y, uint16_t color);
void lcd_fillScreen(lcd_handle_t* obj, uint16_t color);


#ifdef __cplusplus
}
#endif
