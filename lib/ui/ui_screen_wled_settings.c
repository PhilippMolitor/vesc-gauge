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
lv_obj_set_x( ui_wled_settings_switch_enable, -2 );
lv_obj_set_y( ui_wled_settings_switch_enable, 41 );
lv_obj_set_align( ui_wled_settings_switch_enable, LV_ALIGN_CENTER );


ui_wled_settings_label_onoff = lv_label_create(ui_screen_wled_settings);
lv_obj_set_width( ui_wled_settings_label_onoff, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_wled_settings_label_onoff, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_wled_settings_label_onoff, 0 );
lv_obj_set_y( ui_wled_settings_label_onoff, -50 );
lv_obj_set_align( ui_wled_settings_label_onoff, LV_ALIGN_CENTER );
lv_label_set_text(ui_wled_settings_label_onoff,"WLED on/off");
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
lv_label_set_text(ui_wled_settings_label_status,"disconnected");

lv_obj_add_event_cb(ui_wled_settings_switch_enable, ui_event_wled_settings_switch_enable, LV_EVENT_ALL, NULL);
lv_obj_add_event_cb(ui_screen_wled_settings, ui_event_screen_wled_settings, LV_EVENT_ALL, NULL);

}
