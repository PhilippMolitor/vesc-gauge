#pragma once

#include <esp_heap_caps.h>

#include <lvgl.h>

#include "drivers/display/cst820.h"
#include "drivers/display/st7701.h"

#define LCD_WIDTH ESP_PANEL_LCD_WIDTH
#define LCD_HEIGHT ESP_PANEL_LCD_HEIGHT
#define LVGL_BUF_LEN (ESP_PANEL_LCD_WIDTH * ESP_PANEL_LCD_HEIGHT / 5)
#define LVGL_TICK_PERIOD_MS 2

extern lv_disp_drv_t disp_drv;

void display_flush(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p); // Displays LVGL content on the LCD.    This function implements associating LVGL data to the LCD screen
void display_touchpad_update(lv_indev_drv_t* indev_drv, lv_indev_data_t* data); // Read the touchpad
void display_tick(void* arg);
void display_init(void);
