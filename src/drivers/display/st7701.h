#pragma once

#include <inttypes.h>

#include <driver/gpio.h>
#include <driver/spi_master.h>

#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_rgb.h>

#include "../tca9554pwr/tca9554pwr.h"
#include "cst820.h"

#define ST7701_WIDTH (480)
#define ST7701_HEIGHT (480)
#define ST7701_LCD_PIXEL_BITS (16)
#define ST7701_LCD_DATA_WIDTH (16)
#define ST7701_LCD_TIMING_FREQ_HZ (16 * 1000 * 1000)
#define ST7701_LCD_TIMING_HPW (8)
#define ST7701_LCD_TIMING_HBP (10)
#define ST7701_LCD_TIMING_HFP (50)
#define ST7701_LCD_TIMING_VPW (3)
#define ST7701_LCD_TIMING_VBP (8)
#define ST7701_LCD_TIMING_VFP (8)
#define ST7701_LCD_FRAME_BUF_NUM (2)
#define ST7701_BACKLIGHT_PWM_CHANNEL (1)
#define ST7701_BACKLIGHT_PWM_FREQ (20000)
#define ST7701_BACKLIGHT_PWM_RES (10)
#define ST7701_BACKLIGHT_MAX (100)

extern esp_lcd_panel_handle_t panel;

void st7701_configure();
void st7701_setup();
void st7701_draw_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t* data);
void st7701_backlight_init();
void st7701_backlight_set(uint8_t brightness);