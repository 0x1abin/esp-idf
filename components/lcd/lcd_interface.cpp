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

/*SPI Includes*/
#include "driver/spi_master.h"
#include "iot_lcd.h"
#include "Adafruit_GFX.h"

#include "esp_log.h"
#include "esp_system.h"

/*Include desired font here*/
#define PROGMEM
// #include "FreeSans9pt7b.h"

static CEspLcd* lcd_obj = NULL;


// extern "C" void esp_draw(void *pvParameter)
// {
//     /*Initilize ESP32 to scan for Access points*/
//     nvs_flash_init();

//     /*Initialize LCD*/
//     lcd_conf_t lcd_pins = {
//         .lcd_model    = LCD_MOD_ILI9341,
//         .pin_num_miso = GPIO_NUM_19,
//         .pin_num_mosi = GPIO_NUM_23,
//         .pin_num_clk  = GPIO_NUM_18,
//         .pin_num_cs   = GPIO_NUM_14,
//         .pin_num_dc   = GPIO_NUM_27,
//         .pin_num_rst  = GPIO_NUM_33,
//         .pin_num_bckl = GPIO_NUM_32,
//         .clk_freq     = 27 * 1000 * 1000,
//         .rst_active_level = 0,
//         .bckl_active_level = 1,
//         .spi_host = HSPI_HOST,
//         .init_spi_bus = true,
//     };

//     if (lcd_obj == NULL) {
//         lcd_obj = new CEspLcd(&lcd_pins);
//     }
//     // printf("lcd id: 0x%08x\n", lcd_obj->id.id);

//     /*Welcome screen*/
//     int x = 0, y = 0;
//     int dim = 6;
//     uint16_t rand_color;
//     lcd_obj->setRotation(3);
//     for (int i = 0; i < dim; i++) {
//         for (int j = 0; j < 10 - 2 * i; j++) {
//             rand_color = rand();
//             lcd_obj->fillRect(x * 32, y * 24, 32, 24, rand_color);
//             ets_delay_us(10000);
//             x++;
//         }
//         x--;
//         for (int j = 0; j < 10 - 2 * i; j++) {
//             rand_color = rand();
//             lcd_obj->fillRect(x * 32, y * 24, 32, 24, rand_color);
//             ets_delay_us(10000);
//             y++;
//         }
//         y--;
//         for (int j = 0; j < 10 - 2 * i - 1; j++) {
//             rand_color = rand();
//             lcd_obj->fillRect(x * 32, y * 24, 32, 24, rand_color);
//             ets_delay_us(10000);
//             x--;
//         }
//         x++;
//         for (int j = 0; j < 10 - 2 * i - 1; j++) {
//             rand_color = rand();
//             lcd_obj->fillRect((x - 1) * 32, y * 24, 32, 24, rand_color);
//             ets_delay_us(10000);
//             y--;
//         }
//         y++;
//     }
//     vTaskDelay(1000 / portTICK_PERIOD_MS);

//     /*ESPecifications*/
//     lcd_obj->setRotation(1);
//     lcd_obj->fillScreen(COLOR_ESP_BKGD);
//     lcd_obj->setTextSize(1);
//     // lcd_obj->drawBitmap(0, 0, esp_logo, 137, 26);

//     lcd_obj->setTextColor(COLOR_GREEN, COLOR_ESP_BKGD);
//     lcd_obj->setFont(&FreeSans9pt7b);
//     lcd_obj->drawString("CPU",                                     3, 40);
//     lcd_obj->setFont(NULL);
//     lcd_obj->setTextColor(COLOR_YELLOW, COLOR_ESP_BKGD);
//     lcd_obj->drawString("Xtensa Dual-Core 32-bit LX6 MPU",         3, 50);
//     lcd_obj->drawString("Max Clock Speed at 240 MHz & 600 DMIPS ", 3, 60);
//     lcd_obj->drawString("at up to 600 DMIPS",                      3, 70);
//     lcd_obj->drawString("Memory: 520 KiB SRAM",                    3, 80);

//     lcd_obj->setTextColor(COLOR_GREEN, COLOR_ESP_BKGD);
//     lcd_obj->setFont(&FreeSans9pt7b);
//     lcd_obj->drawString("Wireless connectivity",               3,  110);
//     lcd_obj->setFont(NULL);
//     lcd_obj->setTextColor(COLOR_YELLOW, COLOR_ESP_BKGD);
//     lcd_obj->drawString("Wi-Fi: 802.11 b/g/n/e/i",            3,  120);
//     lcd_obj->drawString("Bluetooth: v4.2 BR/EDR and BLE",      3,  130);

//     lcd_obj->setTextColor(COLOR_GREEN, COLOR_ESP_BKGD);
//     lcd_obj->setFont(&FreeSans9pt7b);
//     lcd_obj->drawString("Power Management",                     3, 160);
//     lcd_obj->setFont(NULL);
//     lcd_obj->setTextColor(COLOR_YELLOW, COLOR_ESP_BKGD);
//     lcd_obj->drawString("Internal LDO",                         3, 170);
//     lcd_obj->drawString("Individual power domain for RTC",      3, 180);
//     lcd_obj->drawString("5uA deep sleep current",               3, 190);
//     lcd_obj->drawString("Wake up from GPIO interrupt" ,         3, 200);
//     lcd_obj->drawString("Wake up from timer, ADC measurements", 3, 210);
//     lcd_obj->drawString("Wake up from capacitive sensor intr",  3, 220);

//     lcd_obj->setTextColor(COLOR_GREEN, COLOR_ESP_BKGD);
//     lcd_obj->setFont(&FreeSans9pt7b);
//     lcd_obj->drawString("Security",                               3, 250);
//     lcd_obj->setFont(NULL);
//     lcd_obj->setTextColor(COLOR_YELLOW, COLOR_ESP_BKGD);
//     lcd_obj->drawString("IEEE 802.11 standard security features", 3, 260);
//     lcd_obj->drawString("Secure boot & Flash Encryption",         3, 270);
//     lcd_obj->drawString("Cryptographic Hardware Acceleration",    3, 280);
//     lcd_obj->drawString("AES, RSA, SHA-2, EEC, RNG",              3, 290);
//     lcd_obj->drawString("1024-bit OTP",                           3, 300);

// }


extern "C" {

void lcd_start()
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
        .clk_freq     = 27 * 1000 * 1000,
        .rst_active_level = 0,
        .bckl_active_level = 1,
        .spi_host = HSPI_HOST,
        .init_spi_bus = true,
    };

    if (lcd_obj == NULL) {
        lcd_obj = new CEspLcd(&lcd_pins);
    }
}

void lcd_test()
{
    /*Welcome screen*/
    int x = 0, y = 0;
    int dim = 6;
    uint16_t rand_color;
    lcd_obj->setRotation(3);

    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < 10 - 2 * i; j++) {
            rand_color = rand();
            lcd_obj->fillRect(x * 32, y * 24, 32, 24, rand_color);
            ets_delay_us(10000);
            x++;
        }
        x--;
        for (int j = 0; j < 10 - 2 * i; j++) {
            rand_color = rand();
            lcd_obj->fillRect(x * 32, y * 24, 32, 24, rand_color);
            ets_delay_us(10000);
            y++;
        }
        y--;
        for (int j = 0; j < 10 - 2 * i - 1; j++) {
            rand_color = rand();
            lcd_obj->fillRect(x * 32, y * 24, 32, 24, rand_color);
            ets_delay_us(10000);
            x--;
        }
        x++;
        for (int j = 0; j < 10 - 2 * i - 1; j++) {
            rand_color = rand();
            lcd_obj->fillRect((x - 1) * 32, y * 24, 32, 24, rand_color);
            ets_delay_us(10000);
            y--;
        }
        y++;
    }
}

inline void lcd_drawPixel(int16_t x, int16_t y, uint16_t color)
{
    lcd_obj->drawPixel(x, y, color);
}


} // extern


// color565(uint8_t r, uint8_t g, uint8_t b)