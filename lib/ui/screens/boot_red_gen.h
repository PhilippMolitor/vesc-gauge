/**
 * @file boot_red_gen.h
 */

#ifndef BOOT_RED_H
#define BOOT_RED_H

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

typedef enum {
    BOOT_RED_TIMELINE_BOOT_ANIM = 0,
    _BOOT_RED_TIMELINE_CNT = 1
}boot_red_timeline_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/



lv_obj_t * boot_red_create(void);

/**
 * Get a timeline of a boot_red
 * @param obj          pointer to a boot_red component
 * @param timeline_id  ID of the the timeline
 * @return             pointer to the timeline or NULL if not found
 */
lv_anim_timeline_t * boot_red_get_timeline(lv_obj_t * obj, boot_red_timeline_t timeline_id);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*BOOT_RED_H*/