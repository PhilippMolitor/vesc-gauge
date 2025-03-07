// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.0
// LVGL version: 9.1.0
// Project name: pocketbikedisplay

#include "ui.h"

void ui_screen_wled_settings_screen_init(void)
{
ui_screen_wled_settings = lv_obj_create(NULL);
lv_obj_remove_flag( ui_screen_wled_settings, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_wled_settings_switch_enable = lv_switch_create(ui_screen_wled_settings);
lv_obj_set_width( ui_wled_settings_switch_enable, 141);
lv_obj_set_height( ui_wled_settings_switch_enable, 83);
lv_obj_set_x( ui_wled_settings_switch_enable, 104 );
lv_obj_set_y( ui_wled_settings_switch_enable, -19 );
lv_obj_set_align( ui_wled_settings_switch_enable, LV_ALIGN_CENTER );


ui_wled_settings_label_onoff = lv_label_create(ui_screen_wled_settings);
lv_obj_set_width( ui_wled_settings_label_onoff, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_wled_settings_label_onoff, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_wled_settings_label_onoff, -6 );
lv_obj_set_y( ui_wled_settings_label_onoff, -148 );
lv_obj_set_align( ui_wled_settings_label_onoff, LV_ALIGN_CENTER );
lv_label_set_text(ui_wled_settings_label_onoff,"WLED\non/off");
lv_obj_set_style_text_align(ui_wled_settings_label_onoff, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_wled_settings_label_onoff, &ui_font_font_speed_48pt, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_wled_settings_label_macaddr = lv_label_create(ui_screen_wled_settings);
lv_obj_set_width( ui_wled_settings_label_macaddr, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_wled_settings_label_macaddr, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_wled_settings_label_macaddr, 0 );
lv_obj_set_y( ui_wled_settings_label_macaddr, 127 );
lv_obj_set_align( ui_wled_settings_label_macaddr, LV_ALIGN_CENTER );
lv_label_set_text(ui_wled_settings_label_macaddr,"loading mac address...");

ui_wled_settings_label_status = lv_label_create(ui_screen_wled_settings);
lv_obj_set_width( ui_wled_settings_label_status, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_wled_settings_label_status, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_wled_settings_label_status, -1 );
lv_obj_set_y( ui_wled_settings_label_status, 150 );
lv_obj_set_align( ui_wled_settings_label_status, LV_ALIGN_CENTER );
lv_label_set_text(ui_wled_settings_label_status,"off");

ui_wled_settings_slider_slider_brightness = lv_slider_create(ui_screen_wled_settings);
lv_slider_set_range(ui_wled_settings_slider_slider_brightness, 0,9);
lv_slider_set_value( ui_wled_settings_slider_slider_brightness, 0, LV_ANIM_OFF);
if (lv_slider_get_mode(ui_wled_settings_slider_slider_brightness)==LV_SLIDER_MODE_RANGE ) lv_slider_set_left_value( ui_wled_settings_slider_slider_brightness, 0, LV_ANIM_OFF);
lv_obj_set_width( ui_wled_settings_slider_slider_brightness, 380);
lv_obj_set_height( ui_wled_settings_slider_slider_brightness, 10);
lv_obj_set_x( ui_wled_settings_slider_slider_brightness, 0 );
lv_obj_set_y( ui_wled_settings_slider_slider_brightness, 58 );
lv_obj_set_align( ui_wled_settings_slider_slider_brightness, LV_ALIGN_CENTER );


//Compensating for LVGL9.1 draw crash with bar/slider max value when top-padding is nonzero and right-padding is 0
if (lv_obj_get_style_pad_top(ui_wled_settings_slider_slider_brightness,LV_PART_MAIN) > 0) lv_obj_set_style_pad_right( ui_wled_settings_slider_slider_brightness, lv_obj_get_style_pad_right(ui_wled_settings_slider_slider_brightness,LV_PART_MAIN) + 1, LV_PART_MAIN );
ui_wled_settings_dropdown_channel = lv_dropdown_create(ui_screen_wled_settings);
lv_dropdown_set_dir( ui_wled_settings_dropdown_channel, LV_DIR_RIGHT);
lv_dropdown_set_options( ui_wled_settings_dropdown_channel, "All Channels\nChannel 1\nChannel 2\nChannel 3\nChannel 4\nChannel 5\nChannel 6\nChannel 7\nChannel 8\nChannel 9\nChannel 10\nChannel 11\nChannel 12\nChannel 13\nChannel 14" );
lv_obj_set_width( ui_wled_settings_dropdown_channel, 150);
lv_obj_set_height( ui_wled_settings_dropdown_channel, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_wled_settings_dropdown_channel, -97 );
lv_obj_set_y( ui_wled_settings_dropdown_channel, -22 );
lv_obj_set_align( ui_wled_settings_dropdown_channel, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_wled_settings_dropdown_channel, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags



lv_obj_add_event_cb(ui_wled_settings_switch_enable, ui_event_wled_settings_switch_enable, LV_EVENT_ALL, NULL);
lv_obj_add_event_cb(ui_wled_settings_slider_slider_brightness, ui_event_wled_settings_slider_slider_brightness, LV_EVENT_ALL, NULL);
lv_obj_add_event_cb(ui_wled_settings_dropdown_channel, ui_event_wled_settings_dropdown_channel, LV_EVENT_ALL, NULL);
lv_obj_add_event_cb(ui_screen_wled_settings, ui_event_screen_wled_settings, LV_EVENT_ALL, NULL);

}
