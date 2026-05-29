/**
 * @file ui_gen.h
 */

#ifndef UI_GEN_H
#define UI_GEN_H

#ifndef UI_SUBJECT_STRING_LENGTH
#define UI_SUBJECT_STRING_LENGTH 256
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
    #include "lvgl_private.h"
#else
    #include "lvgl/lvgl.h"
    #include "lvgl/lvgl_private.h"
#endif

#ifdef LV_USE_XML
    #include "lv_xml/lv_xml.h"
#endif



/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/

/*-------------------
 * Permanent screens
 *------------------*/

/*----------------
 * Global styles
 *----------------*/

/*----------------
 * Fonts
 *----------------*/

/*----------------
 * Images
 *----------------*/

/*----------------
 * Subjects
 *----------------*/

extern lv_subject_t bat_voltage;
extern lv_subject_t bat_cells;
extern lv_subject_t bat_cell_voltage_min;
extern lv_subject_t bat_cell_voltage_max;
extern lv_subject_t bat_cell_voltage_diff;
extern lv_subject_t bat_soc;
extern lv_subject_t speed_ms;
extern lv_subject_t esc_power_w;
extern lv_subject_t periph_headlight;
extern lv_subject_t periph_taillight;
extern lv_subject_t periph_breaklight;
extern lv_subject_t periph_turnsignal;
extern lv_subject_t periph_lights_enabled;
extern lv_subject_t periph_breaklight_enabled;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/

/**
 * Initialize the component library
 */

void ui_init_gen(const char * asset_path);

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 **********************/

/*Include all the widgets, components and screens of this library*/
#include "screens/boot_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_GEN_H*/