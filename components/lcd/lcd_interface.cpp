/* ILI9341_SPI example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
 Some info about the ILI9341: It has an C/D line, which is connected to a GPIO here. It expects this
 line to be low for a command and high for data. We use a pre-transmit callback to control that line

 every transaction has as the user-definable argument the needed state of the D/C line and just
 before the transaction is sent, the callback will set this line to the correct state.
 Note: If not using ESP WROVER KIT, Users must change the pins by setting macro in file spi_ili.c

 To change the fonts style include the header of your desired font from the "component/includes" folder
 and simply pass the address of its GFXfont to setFont API
*/

/*C Includes*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*RTOS Includes*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"

#include "esp_log.h"
#include "esp_system.h"

/*SPI Includes*/
#include "driver/spi_master.h"
#include "iot_lcd.h"
#include "Adafruit_GFX.h"
#include "lcd_interface.h"

/*Include desired font here*/
#define PROGMEM
// #include "FreeSans9pt7b.h"


extern "C" {


lcd_handle_t* lcd_create_obj(uint8_t spi_host, uint8_t init_bus)
{
    /*Initialize LCD*/
    lcd_conf_t lcd_pins = {
        .lcd_model    = LCD_MOD_ILI9341,
        .pin_num_miso = GPIO_NUM_19,
        .pin_num_mosi = GPIO_NUM_23,
        .pin_num_clk  = GPIO_NUM_18,
        .pin_num_cs   = GPIO_NUM_14,
        .pin_num_dc   = GPIO_NUM_27,
        .pin_num_rst  = GPIO_NUM_33,
        .pin_num_bckl = GPIO_NUM_32,
        .clk_freq     = 40 * 1000 * 1000,
        .rst_active_level = 0,
        .bckl_active_level = 1,
        .spi_host = (spi_host_device_t)spi_host,
        .init_spi_bus = (bool)init_bus,
    };

    return (lcd_handle_t*)(new CEspLcd(&lcd_pins));
}

// uint16_t lcd_color565(lcd_handle_t* lcd_obj, uint8_t r, uint8_t g, uint8_t b)
// {
//     ((CEspLcd*)lcd_obj)->color565(r, g, b);
// }

void lcd_drawPixel(lcd_handle_t* lcd_obj, int16_t x, int16_t y, uint16_t color)
{
    // CEspLcd* lcd_obj = (CEspLcd*)obj;
    ((CEspLcd*)lcd_obj)->drawPixel(x, y, color);
}

void lcd_fillScreen(lcd_handle_t* lcd_obj, uint16_t color)
{
    ((CEspLcd*)lcd_obj)->fillScreen(color);
}





} // extern


// color565(uint8_t r, uint8_t g, uint8_t b)