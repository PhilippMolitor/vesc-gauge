#pragma once

#include <inttypes.h>

#define ST7701_WIDTH (480)
#define ST7701_HEIGHT (480)
#define ST7701_LCD_PIXEL_BITS (16)
#define ST7701_BACKLIGHT_MAX (100)

void st7701_init(void* buf_a, void* buf_b);
void st7701_draw_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t* data);
void st7701_backlight_init();
void st7701_backlight_set(uint8_t brightness);