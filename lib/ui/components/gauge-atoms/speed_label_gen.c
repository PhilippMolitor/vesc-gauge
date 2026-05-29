/**
 * @file speed_label_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "speed_label_gen.h"
#include "../../ui.h"

/*********************
 *      DEFINES
 *********************/

#define FORMAT "%0.f"

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

lv_obj_t * speed_label_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    lv_obj_t * lv_label_0 = lv_label_create(parent);
    lv_obj_set_name_static(lv_label_0, "speed_label_#");
    lv_label_set_text(lv_label_0, "");

    lv_obj_t * lv_label_1 = lv_label_create(lv_label_0);
    lv_label_bind_text(lv_label_1, &speed_ms, FORMAT);
    lv_obj_bind_flag_if_not_eq(lv_label_1, &ui_speed_unit, LV_OBJ_FLAG_HIDDEN, 0);
    
    lv_obj_t * lv_label_2 = lv_label_create(lv_label_0);
    lv_label_bind_text(lv_label_2, &speed_kmh, FORMAT);
    lv_obj_bind_flag_if_not_eq(lv_label_2, &ui_speed_unit, LV_OBJ_FLAG_HIDDEN, 1);
    
    lv_obj_t * lv_label_3 = lv_label_create(lv_label_0);
    lv_label_bind_text(lv_label_3, &speed_mph, FORMAT);
    lv_obj_bind_flag_if_not_eq(lv_label_3, &ui_speed_unit, LV_OBJ_FLAG_HIDDEN, 2);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_label_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

