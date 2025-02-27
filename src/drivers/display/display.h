#pragma once

#include <lvgl.h>

#include "cst820.h"
#include "st7701.h"

#define FB_SIZE (ST7701_WIDTH * ST7701_HEIGHT / 6)

void display_flush(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map);
void display_touchpad_update(lv_indev_t* drv, lv_indev_data_t* data);
void display_init(void);
