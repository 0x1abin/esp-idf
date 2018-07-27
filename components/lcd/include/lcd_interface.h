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

// Color definitions
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F

typedef void* lcd_handle_t;

lcd_handle_t* lcd_create_obj(uint8_t spi_host, uint8_t init_bus);

// uint16_t lcd_color565(lcd_handle_t* lcd_obj, uint8_t r, uint8_t g, uint8_t b);
void lcd_setCursor(lcd_handle_t* lcd_obj, int16_t x, int16_t y);
int16_t lcd_getCursorX(lcd_handle_t* lcd_obj);
int16_t lcd_getCursorY(lcd_handle_t* lcd_obj);
void lcd_setRotation(lcd_handle_t* lcd_obj, uint8_t m);
void lcd_drawPixel(lcd_handle_t* lcd_obj, int16_t x, int16_t y, uint16_t color);
void lcd_fillScreen(lcd_handle_t* obj, uint16_t color);
void lcd_drawLine(lcd_handle_t* lcd_obj, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void lcd_drawTriangle(lcd_handle_t* lcd_obj, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void lcd_fillTriangle(lcd_handle_t* lcd_obj, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void lcd_fillCircle(lcd_handle_t* lcd_obj, int16_t x, int16_t y, int16_t r, uint16_t color);
void lcd_drawCircle(lcd_handle_t* lcd_obj, int16_t x, int16_t y, int16_t r, uint16_t color);
void lcd_drawString(lcd_handle_t* lcd_obj, const char *string, uint16_t x, uint16_t y);

#ifdef __cplusplus
}
#endif
