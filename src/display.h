#pragma once

#include <lvgl.h>

#include "drivers/display/cst820.h"
#include "drivers/display/st7701.h"

#define FB_SIZE (ESP_PANEL_LCD_WIDTH * ESP_PANEL_LCD_HEIGHT / 6)

void display_flush(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map);
void display_touchpad_update(lv_indev_t* drv, lv_indev_data_t* data);
void display_tick(void* arg);
void display_init(void);
