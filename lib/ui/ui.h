// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.0
// LVGL version: 9.1.0
// Project name: pocketbikedisplay

#ifndef _POCKETBIKEDISPLAY_UI_H
#define _POCKETBIKEDISPLAY_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined __has_include
  #if __has_include("lvgl.h")
    #include "lvgl.h"
  #elif __has_include("lvgl/lvgl.h")
    #include "lvgl/lvgl.h"
  #else
    #include "lvgl.h"
  #endif
#else
  #include "lvgl.h"
#endif

#include "ui_helpers.h"
#include "ui_events.h"
#include "ui_theme_manager.h"
#include "ui_themes.h"

// SCREEN: ui_screen_boot
void ui_screen_boot_screen_init(void);
extern lv_obj_t *ui_screen_boot;
extern lv_obj_t *ui_boot_label_booting;
void ui_event_boot_button_continue( lv_event_t * e);
extern lv_obj_t *ui_boot_button_continue;
// CUSTOM VARIABLES

// SCREEN: ui_screen_main
void ui_screen_main_screen_init(void);
void ui_event_screen_main( lv_event_t * e);
extern lv_obj_t *ui_screen_main;
extern lv_obj_t *ui_main_arc_arc_battery_percent;
extern lv_obj_t *ui_main_arc_arc_esc_power;
extern lv_obj_t *ui_main_container_speed_container;
extern lv_obj_t *ui_main_label_speed_value;
extern lv_obj_t *ui_main_label_speed_label;
// CUSTOM VARIABLES

// SCREEN: ui_screen_wled_settings
void ui_screen_wled_settings_screen_init(void);
void ui_event_screen_wled_settings( lv_event_t * e);
extern lv_obj_t *ui_screen_wled_settings;
void ui_event_wled_settings_switch_enable( lv_event_t * e);
extern lv_obj_t *ui_wled_settings_switch_enable;
extern lv_obj_t *ui_wled_settings_label_onoff;
extern lv_obj_t *ui_wled_settings_label_macaddr;
extern lv_obj_t *ui_wled_settings_label_status;
void ui_event_wled_settings_button_brightness_dec( lv_event_t * e);
extern lv_obj_t *ui_wled_settings_button_brightness_dec;
extern lv_obj_t *ui_wled_settings_label_brightness_dec_minus;
void ui_event_wled_settings_button_brightness_inc( lv_event_t * e);
extern lv_obj_t *ui_wled_settings_button_brightness_inc;
extern lv_obj_t *ui_wled_settings_label_brightness_inc_plus;
// CUSTOM VARIABLES

// SCREEN: ui_screen_lights_control
void ui_screen_lights_control_screen_init(void);
void ui_event_screen_lights_control( lv_event_t * e);
extern lv_obj_t *ui_screen_lights_control;
// CUSTOM VARIABLES

// EVENTS
extern lv_obj_t *ui_startevents____initial_actions0;

// FONTS
LV_FONT_DECLARE( ui_font_font_speed_136pt);
LV_FONT_DECLARE( ui_font_font_speed_48pt);

// UI INIT
void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
