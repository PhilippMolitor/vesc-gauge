/**
 * @file gauge_driftline_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "gauge_driftline_gen.h"
#include "../../ui.h"

/*********************
 *      DEFINES
 *********************/

#define SOC_LOW lv_color_hex3(0xf00)

#define SOC_MID lv_color_hex3(0xff0)

#define SOC_HIGH lv_color_hex3(0x0f0)

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * gauge_driftline_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t speed_label_main;
    static lv_style_t speed_label_unit;
    static lv_style_t stat_arc_main;
    static lv_style_t stat_arc_indicator;
    static lv_style_t stat_arc_knob;
    static lv_style_t stat_soc_low;
    static lv_style_t stat_soc_mid;
    static lv_style_t stat_soc_high;

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&speed_label_main);
        lv_style_set_text_color(&speed_label_main, FG_MAIN);

        lv_style_init(&speed_label_unit);
        lv_style_set_text_color(&speed_label_unit, FG_DIMMED);

        lv_style_init(&stat_arc_main);
        lv_style_set_align(&stat_arc_main, LV_ALIGN_CENTER);
        lv_style_set_width(&stat_arc_main, 474);
        lv_style_set_height(&stat_arc_main, 474);
        lv_style_set_radius(&stat_arc_main, 0);
        lv_style_set_arc_opa(&stat_arc_main, 0);
        lv_style_set_arc_rounded(&stat_arc_main, false);

        lv_style_init(&stat_arc_indicator);
        lv_style_set_arc_color(&stat_arc_indicator, lv_color_hex3(0x0f0));
        lv_style_set_arc_rounded(&stat_arc_indicator, false);
        lv_style_set_arc_width(&stat_arc_indicator, 6);

        lv_style_init(&stat_arc_knob);
        lv_style_set_bg_opa(&stat_arc_knob, 0);

        lv_style_init(&stat_soc_low);
        lv_style_set_arc_color(&stat_soc_low, SOC_LOW);

        lv_style_init(&stat_soc_mid);
        lv_style_set_arc_color(&stat_soc_mid, SOC_MID);

        lv_style_init(&stat_soc_high);
        lv_style_set_arc_color(&stat_soc_high, SOC_HIGH);

        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(parent);
    lv_obj_set_name_static(lv_obj_0, "gauge_driftline_#");
    lv_obj_set_width(lv_obj_0, lv_pct(100));
    lv_obj_set_height(lv_obj_0, lv_pct(100));
    lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex3(0x000), 0);

    lv_obj_t * speed_label_0 = speed_label_create(lv_obj_0);
    lv_obj_set_style_text_font(speed_label_0, crixus_italic_4xl, 0);
    lv_obj_set_align(speed_label_0, LV_ALIGN_CENTER);
    lv_obj_set_style_margin_left(speed_label_0, -76, 0);
    lv_obj_set_style_pad_right(speed_label_0, 38, 0);
    lv_obj_add_style(speed_label_0, &speed_label_main, LV_PART_MAIN);
    
    lv_obj_t * speed_unit_label_0 = speed_unit_label_create(lv_obj_0);
    lv_obj_set_style_text_font(speed_unit_label_0, crixus_italic_xxl, 0);
    lv_obj_set_align(speed_unit_label_0, LV_ALIGN_CENTER);
    lv_obj_set_y(speed_unit_label_0, 110);
    lv_obj_add_style(speed_unit_label_0, &speed_label_unit, LV_PART_MAIN);
    
    lv_obj_t * lv_arc_0 = lv_arc_create(lv_obj_0);
    lv_arc_set_start_angle(lv_arc_0, 100);
    lv_arc_set_end_angle(lv_arc_0, 260);
    lv_arc_set_bg_start_angle(lv_arc_0, 100);
    lv_arc_set_bg_end_angle(lv_arc_0, 260);
    lv_obj_set_flag(lv_arc_0, LV_OBJ_FLAG_CLICKABLE, false);
    lv_arc_set_min_value(lv_arc_0, 0);
    lv_arc_set_max_value(lv_arc_0, 100);
    lv_arc_set_value(lv_arc_0, 20);
    lv_obj_add_style(lv_arc_0, &stat_arc_main, LV_PART_MAIN);
    lv_obj_add_style(lv_arc_0, &stat_arc_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(lv_arc_0, &stat_arc_knob, LV_PART_KNOB);
    
    lv_obj_t * lv_arc_1 = lv_arc_create(lv_obj_0);
    lv_arc_set_bg_start_angle(lv_arc_1, 280);
    lv_arc_set_bg_end_angle(lv_arc_1, 80);
    lv_arc_set_mode(lv_arc_1, LV_ARC_MODE_REVERSE);
    lv_obj_set_flag(lv_arc_1, LV_OBJ_FLAG_CLICKABLE, false);
    lv_arc_set_min_value(lv_arc_1, 0);
    lv_arc_set_max_value(lv_arc_1, 100);
    lv_arc_bind_value(lv_arc_1, &bat_soc_percent);
    lv_obj_add_style(lv_arc_1, &stat_arc_main, LV_PART_MAIN);
    lv_obj_add_style(lv_arc_1, &stat_arc_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(lv_arc_1, &stat_arc_knob, LV_PART_KNOB);
    lv_obj_bind_style(lv_arc_1, &stat_soc_low, LV_PART_INDICATOR, &bat_soc_state, 0);
    lv_obj_bind_style(lv_arc_1, &stat_soc_mid, LV_PART_INDICATOR, &bat_soc_state, 1);
    lv_obj_bind_style(lv_arc_1, &stat_soc_high, LV_PART_INDICATOR, &bat_soc_state, 2);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

