/**
 * @file speed_unit_label_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "speed_unit_label_gen.h"
#include "../ui.h"

/*********************
 *      DEFINES
 *********************/

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

lv_obj_t * speed_unit_label_create(lv_obj_t * parent)
{
    LV_TRACE_OBJ_CREATE("begin");

    lv_obj_t * lv_label_0 = lv_label_create(parent);
    lv_obj_set_name_static(lv_label_0, "speed_unit_label_#");
    lv_label_set_text(lv_label_0, "");

    lv_obj_t * lv_label_1 = lv_label_create(lv_label_0);
    lv_label_set_text(lv_label_1, "m/s");
    lv_obj_bind_flag_if_not_eq(lv_label_1, &bat_cells, LV_OBJ_FLAG_HIDDEN, 0);
    
    lv_obj_t * lv_label_2 = lv_label_create(lv_label_0);
    lv_label_set_text(lv_label_2, "KM/H");
    lv_obj_bind_flag_if_not_eq(lv_label_2, &ui_speed_unit, LV_OBJ_FLAG_HIDDEN, 1);
    
    lv_obj_t * lv_label_3 = lv_label_create(lv_label_0);
    lv_label_set_text(lv_label_3, "MPH");
    lv_obj_bind_flag_if_not_eq(lv_label_3, &ui_speed_unit, LV_OBJ_FLAG_HIDDEN, 2);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_label_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

