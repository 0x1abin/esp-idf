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
// #include "py/mpprint.h"
// #include "extmod/vfs_native.h"

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

void lcd_drawLine(lcd_handle_t* lcd_obj, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    ((CEspLcd*)lcd_obj)->drawLine(x0, y0, x1, y1, color);
}

void lcd_drawTriangle(lcd_handle_t* lcd_obj, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    ((CEspLcd*)lcd_obj)->drawTriangle(x0, y0, x1, y1, x2, y2, color);
}

void lcd_fillTriangle(lcd_handle_t* lcd_obj, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    ((CEspLcd*)lcd_obj)->fillTriangle(x0, y0, x1, y1, x2, y2, color);
}

void lcd_drawCircle(lcd_handle_t* lcd_obj, int16_t x, int16_t y, int16_t r, uint16_t color)
{
    ((CEspLcd*)lcd_obj)->drawCircle(x, y, r, color);
}

void lcd_fillCircle(lcd_handle_t* lcd_obj, int16_t x, int16_t y, int16_t r, uint16_t color)
{
    ((CEspLcd*)lcd_obj)->fillCircle(x, y, r, color);
}

// void lcd_drawRect(lcd_handle_t* lcd_obj, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
// {
//     ((CEspLcd*)lcd_obj)->drawRect(x, y, w, h, color);
// }

// void lcd_fillRect(lcd_handle_t* lcd_obj, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
// {
//     ((CEspLcd*)lcd_obj)->fillRect(x, y, w, h, color);
// }

// void lcd_drawRoundRect(lcd_handle_t* lcd_obj, int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
// {
//     ((CEspLcd*)lcd_obj)->drawRoundRect(x, y, w, h, r, color);
// }

// void lcd_fillRoundRect(lcd_handle_t* lcd_obj, int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
// {
//     ((CEspLcd*)lcd_obj)->fillRoundRect(x, y, w, h, r, color);
// }

void lcd_drawString(lcd_handle_t* lcd_obj, const char *string, uint16_t x, uint16_t y)
{
    ((CEspLcd*)lcd_obj)->drawString(string, x, y);
}

void lcd_setCursor(lcd_handle_t* lcd_obj, int16_t x, int16_t y)
{
    ((CEspLcd*)lcd_obj)->setCursor(x, y);
}

int16_t lcd_getCursorX(lcd_handle_t* lcd_obj)
{
    return ((CEspLcd*)lcd_obj)->getCursorX();
}

int16_t lcd_getCursorY(lcd_handle_t* lcd_obj)
{
    return ((CEspLcd*)lcd_obj)->getCursorY();
}

void lcd_setRotation(lcd_handle_t* lcd_obj, uint8_t m)
{
    ((CEspLcd*)lcd_obj)->setRotation(m);
}

void lcd_setTextColor(lcd_handle_t* lcd_obj, uint16_t c)
{
    ((CEspLcd*)lcd_obj)->setTextColor(c);
}

void lcd_setTextbgColor(lcd_handle_t* lcd_obj, uint16_t c, uint16_t b)
{
    ((CEspLcd*)lcd_obj)->setTextColor(c, b);
}

//====================================================================================
// int lcd_bmp_image(lcd_handle_t* lcd_obj, int16_t x, int16_t y, uint8_t scale, char *fname, uint8_t *imgbuf, int size)
// {
// 	FILE *fhndl = NULL;
// 	struct stat sb;
// 	int i, err=0;
// 	int16_t img_wraw, img_hraw, img_xstart, img_w, img_ystart, img_h;
// 	int img_pos, img_pix_pos, scan_lines, rd_len;
// 	uint8_t tmpc;
// 	uint16_t wtemp;
// 	uint32_t temp;
// 	int disp_xstart, disp_xend, disp_ystart, disp_yend;
// 	uint8_t buf[56];
// 	char err_buf[64];
// 	uint8_t *line_buf[2] = {NULL,NULL};
// 	uint8_t lb_idx = 0;
// 	uint8_t *scale_buf = NULL;
// 	uint8_t scale_pix;
// 	uint16_t co[3] = {0,0,0};			// RGB sum
// 	uint8_t npix;

// 	if (scale > 7) scale = 7;
// 	scale_pix = scale+1;	// scale factor ( 1~8 )

//     if (fname) {
//     	// * File name is given, reading image from file
//     	if (stat(fname, &sb) != 0) {
// 			sprintf(err_buf, "the image file does not exist");
//     		err = -1;
//     		goto exit;
//     	}
//     	size = sb.st_size;
// 		fhndl = fopen(fname, "r");
// 		if (!fhndl) {
// 			sprintf(err_buf, "can not open image file");
// 			err = -2;
// 			goto exit;
// 		}

// 		i = fread(buf, 1, 54, fhndl);  // read header
//     }
//     else {
//     	// * Reading image from buffer
//     	if ((imgbuf) && (size > 54)) {
//     		memcpy(buf, imgbuf, 54);
//     		i = 54;
//     	}
//     	else i = 0;
//     }

//     // Check if the image file is on sdcard
//     image_trans = file_noton_spi_sdcard(fname);

//     sprintf(err_buf, "reading header");
// 	if (i != 54) {err = -3;	goto exit;}

// 	// ** Check image header and get image properties
// 	if ((buf[0] != 'B') || (buf[1] != 'M')) {err=-4; goto exit;} // accept only images with 'BM' id

// 	memcpy(&temp, buf+2, 4);				// file size
// 	if (temp != size) {err=-5; goto exit;}

// 	memcpy(&img_pos, buf+10, 4);			// start of pixel data

// 	memcpy(&temp, buf+14, 4);				// BMP header size
// 	if (temp != 40) {err=-6; goto exit;}

// 	memcpy(&wtemp, buf+26, 2);				// the number of color planes
// 	if (wtemp != 1) {err=-7; goto exit;}

// 	memcpy(&wtemp, buf+28, 2);				// the number of bits per pixel
// 	if (wtemp != 24) {err=-8; goto exit;}

// 	memcpy(&temp, buf+30, 4);				// the compression method being used
// 	if (temp != 0) {err=-9; goto exit;}

// 	memcpy(&img_wraw, buf+18, 4);			// the bitmap width in pixels
// 	memcpy(&img_hraw, buf+22, 4);			// the bitmap height in pixels


// 	// * scale image dimensions

// 	img_w = img_wraw / scale_pix;		// image display horizontal size
// 	img_h = img_hraw / scale_pix;		// image display vertical size

// 	if (x == CENTER) x = ((dispWin.x2 - dispWin.x1 + 1 - img_w) / 2) + dispWin.x1;
// 	else if (x == RIGHT) x = dispWin.x2 + 1 - img_w;

// 	if (y == CENTER) y = ((dispWin.y2 - dispWin.y1 + 1 - img_h) / 2) + dispWin.y1;
// 	else if (y == BOTTOM) y = dispWin.y2 + 1 - img_h;

// 	if ((x < ((dispWin.x2 + 1) * -1)) || (x > (dispWin.x2 + 1)) || (y < ((dispWin.y2 + 1) * -1)) || (y > (dispWin.y2 + 1))) {
// 		sprintf(err_buf, "out of display area (%d,%d", x, y);
// 		err = -10;
// 		goto exit;
// 	}

// 	// ** set display and image areas
// 	if (x < dispWin.x1) {
// 		disp_xstart = dispWin.x1;
// 		img_xstart = -x;	// image pixel line X offset
// 		img_w += x;
// 	}
// 	else {
// 		disp_xstart = x;
// 		img_xstart = 0;
// 	}
// 	if (y < dispWin.y1) {
// 		disp_ystart = dispWin.y1;
// 		img_ystart = -y;	// image pixel line Y offset
// 		img_h += y;
// 	}
// 	else {
// 		disp_ystart = y;
// 		img_ystart = 0;
// 	}
// 	disp_xend = disp_xstart + img_w - 1;
// 	disp_yend = disp_ystart + img_h - 1;
// 	if (disp_xend > dispWin.x2) {
// 		disp_xend = dispWin.x2;
// 		img_w = disp_xend - disp_xstart + 1;
// 	}
// 	if (disp_yend > dispWin.y2) {
// 		disp_yend = dispWin.y2;
// 		img_h = disp_yend - disp_ystart + 1;
// 	}

// 	if ((img_w < 8) || (img_h < 8) || (img_xstart >= (img_wraw-2)) || ((img_hraw - img_ystart) < 2)) {
// 		sprintf(err_buf, "image too small");
// 		err = -11;
// 		goto exit;
// 	}

// 	// ** Allocate memory for 2 lines of image pixels
// 	line_buf[0] = malloc(img_wraw*3);
// 	if (line_buf[0] == NULL) {
// 	    sprintf(err_buf, "allocating line buffer #1");
// 		err=-12;
// 		goto exit;
// 	}

// 	line_buf[1] = malloc(img_wraw*3);
// 	if (line_buf[1] == NULL) {
// 	    sprintf(err_buf, "allocating line buffer #2");
// 		err=-13;
// 		goto exit;
// 	}

// 	if (scale) {
// 		// Allocate memory for scale buffer
// 		rd_len = img_w * 3 * scale_pix;
// 		scale_buf = malloc(rd_len*scale_pix);
// 		if (scale_buf == NULL) {
// 			sprintf(err_buf, "allocating scale buffer");
// 			err=-14;
// 			goto exit;
// 		}
// 	}
// 	else rd_len = img_w * 3;

// 	// ** ***************************************************** **
// 	// ** BMP images are stored in file from LAST to FIRST line **
// 	// ** ***************************************************** **

// 	/* Used variables:
// 		img_wraw		horizontal image size in pixels
// 		img_hraw		number of image lines
// 		img_w 		image display horizontal scaled size in pixels
// 		img_h		image display vertical scaled size in pixels
// 		img_xstart		first pixel in line to be displayed
// 		img_ystart		first image line to be displayed
// 		img_w		number of pixels in image line to be displayed, starting with 'img_xstart'
// 		img_h		number of lines in image to be displayed, starting with 'img_ystart'
// 		rd_len			length of color data which are read from image line in bytes
// 	 */

// 	// Set position in image to the first color data (beginning of the LAST line)
// 	img_pos += (img_ystart * (img_wraw*3));
// 	if (fhndl) {
// 		if (fseek(fhndl, img_pos, SEEK_SET) != 0) {
// 			sprintf(err_buf, "file seek at %d", img_pos);
// 			err = -15;
// 			goto exit;
// 		}
// 	}

// 	if (image_debug) mp_printf(&mp_plat_print, "BMP: image size: (%d,%d) scale: %d disp size: (%d,%d) img xofs: %d img yofs: %d at: %d,%d; line buf: 2* %d scale buf: %d\r\n",
// 			img_wraw, img_hraw, scale_pix, img_w, img_h, img_xstart, img_ystart, disp_xstart, disp_ystart, img_wraw*3, ((scale) ? (rd_len*scale_pix) : 0));

// 	// * Select the display
// 	if (image_trans) disp_select();

// 	while ((disp_yend >= disp_ystart) && ((img_pos + (img_wraw*3)) <= size)) {
// 		if (img_pos > size) {
// 			sprintf(err_buf, "EOF reached: %d > %d", img_pos, size);
// 			err = -16;
// 			goto exit1;
// 		}
// 		if (scale == 0) {
// 			// Read the line of color data into color buffer
// 			if (fhndl) {
// 				i = fread(line_buf[lb_idx], 1, img_wraw*3, fhndl);  // read line from file
// 				if (i != (img_wraw*3)) {
// 					sprintf(err_buf, "file read at %d (%d<>%d)", img_pos, i, img_wraw*3);
// 					err = -16;
// 					goto exit1;
// 				}
// 			}
// 			else memcpy(line_buf[lb_idx], imgbuf+img_pos, img_wraw*3);

// 			if (img_xstart > 0)	memmove(line_buf[lb_idx], line_buf[lb_idx]+(img_xstart*3), rd_len);
// 			// Convert colors BGR-888 (BMP) -> RGB-888 (DISPLAY) ===
// 			for (i=0; i < rd_len; i += 3) {
// 				tmpc = line_buf[lb_idx][i+2] & 0xfc;				// save R
// 				line_buf[lb_idx][i+2] = line_buf[lb_idx][i] & 0xfc;	// B -> R
// 				line_buf[lb_idx][i] = tmpc;							// R -> B
// 				line_buf[lb_idx][i+1] &= 0xfc;						// G
// 			}
// 			img_pos += (img_wraw*3);
// 		}
// 		else {
// 			// scale image, read 'scale_pix' lines and find the average color
// 			for (scan_lines=0; scan_lines<scale_pix; scan_lines++) {
// 				if (img_pos > size) break;
// 				if (fhndl) {
// 					i = fread(line_buf[lb_idx], 1, img_wraw*3, fhndl);  // read line from file
// 					if (i != (img_wraw*3)) {
// 						sprintf(err_buf, "file read at %d (%d<>%d)", img_pos, i, img_wraw*3);
// 						err = -17;
// 						goto exit1;
// 					}
// 				}
// 				else memcpy(line_buf[lb_idx], imgbuf+img_pos, img_wraw*3);
// 				img_pos += (img_wraw*3);

// 				// copy only data which are displayed to scale buffer
// 				memcpy(scale_buf + (rd_len * scan_lines), line_buf[lb_idx]+img_xstart, rd_len);
// 			}

// 			// Populate display line buffer
// 			for (int n=0;n<(img_w*3);n += 3) {
// 				memset(co, 0, sizeof(co));	// initialize color sum
// 				npix = 0;					// initialize number of pixels in scale rectangle

// 				// sum all pixels in scale rectangle
// 				for (int sc_line=0; sc_line<scan_lines; sc_line++) {
// 					// Get colors position in scale buffer
// 					img_pix_pos = (rd_len * sc_line) + (n * scale_pix);

// 					for (int sc_col=0; sc_col<scale_pix; sc_col++) {
// 						co[0] += scale_buf[img_pix_pos];
// 						co[1] += scale_buf[img_pix_pos + 1];
// 						co[2] += scale_buf[img_pix_pos + 2];
// 						npix++;
// 					}
// 				}
// 				// Place the average in display buffer, convert BGR-888 (BMP) -> RGB-888 (DISPLAY)
// 				line_buf[lb_idx][n+2] = (uint8_t)(co[0] / npix);	// B
// 				line_buf[lb_idx][n+1] = (uint8_t)(co[1] / npix);	// G
// 				line_buf[lb_idx][n] = (uint8_t)(co[2] / npix);		// R
// 			}
// 		}

// 		if (image_trans) {
// 			wait_trans_finish(1);
// 			send_data(disp_xstart, disp_yend, disp_xend, disp_yend, img_w, (color_t *)line_buf[lb_idx], 0);
// 		}
// 		else {
// 			disp_select();
// 			send_data(disp_xstart, disp_yend, disp_xend, disp_yend, img_w, (color_t *)line_buf[lb_idx], 0);
// 			wait_trans_finish(1);
// 			disp_deselect();
// 		}
// 		lb_idx = (lb_idx + 1) & 1;  // change buffer

// 		disp_yend--;
// 	}
// 	wait_trans_finish(1);
// 	err = 0;
// exit1:
// 	if (image_trans) disp_deselect();
// exit:
// 	if (scale_buf) free(scale_buf);
// 	if (line_buf[0]) free(line_buf[0]);
// 	if (line_buf[1]) free(line_buf[1]);
// 	if (fhndl) fclose(fhndl);
// 	if ((err) && (image_debug)) mp_printf(&mp_plat_print, "Error: %d [%s]\r\n", err, err_buf);

// 	return err;
// }

} // extern

// color565(uint8_t r, uint8_t g, uint8_t b)