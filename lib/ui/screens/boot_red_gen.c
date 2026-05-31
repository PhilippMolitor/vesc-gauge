/**
 * @file boot_red_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "boot_red_gen.h"
#include "../ui.h"

/*********************
 *      DEFINES
 *********************/

/**
 * Logo background fade duration
 */
#define LOGO_BG_FADEIN_DURATION 700

/**
 * Hero rect fade+slide duration
 */
#define LOGO_FADEIN_DURATION 1500

/**
 * Label fade-in duration
 */
#define LABEL_DURATION 500

/**
 * Fade-out duration for label and spinner
 */
#define FADEOUT_DURATION 1000

/**
 * Delay before label and spinner appear
 */
#define LOGO_DELAY 1000

/**
 * Delay before label and spinner appear
 */
#define LABEL_DELAY 3000

/**
 * Delay before fade-out starts
 */
#define FADEOUT_DELAY 7000

/**
 * Delay before switching to next screen
 */
#define SCREEN_DELAY 8000

/**
 * Screen fade-in transition duration
 */
#define SCREEN_TRANSITION_DURATION 2000

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

static lv_anim_timeline_t * timeline_boot_anim_create(lv_obj_t * obj);
static void free_timeline_event_cb(lv_event_t * e);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * boot_red_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t spinner_main;
    static lv_style_t spinner_indicator;
    static lv_style_t spinner_knob;

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&spinner_main);
        lv_style_set_align(&spinner_main, LV_ALIGN_CENTER);
        lv_style_set_radius(&spinner_main, 0);
        lv_style_set_arc_opa(&spinner_main, 0);
        lv_style_set_arc_rounded(&spinner_main, false);

        lv_style_init(&spinner_indicator);
        lv_style_set_arc_color(&spinner_indicator, BG_ACCENT);
        lv_style_set_arc_rounded(&spinner_indicator, false);
        lv_style_set_arc_width(&spinner_indicator, 5);
        lv_style_set_radius(&spinner_indicator, 0);

        lv_style_init(&spinner_knob);
        lv_style_set_bg_opa(&spinner_knob, 0);

        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
    lv_obj_set_name_static(lv_obj_0, "boot_red_#");
    lv_obj_set_style_bg_color(lv_obj_0, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(lv_obj_0, 255, 0);
    lv_obj_set_width(lv_obj_0, 480);
    lv_obj_set_height(lv_obj_0, lv_pct(100));

    lv_obj_t * logo_bg = lv_obj_create(lv_obj_0);
    lv_obj_set_name(logo_bg, "logo_bg");
    lv_obj_set_width(logo_bg, lv_pct(100));
    lv_obj_set_height(logo_bg, 86);
    lv_obj_set_style_translate_y(logo_bg, -17, 0);
    lv_obj_set_style_bg_color(logo_bg, BG_ACCENT, 0);
    lv_obj_set_style_border_width(logo_bg, 0, 0);
    lv_obj_set_align(logo_bg, LV_ALIGN_CENTER);
    lv_obj_set_style_radius(logo_bg, 0, 0);
    
    lv_obj_t * logo = lv_image_create(lv_obj_0);
    lv_obj_set_name(logo, "logo");
    lv_obj_set_width(logo, lv_pct(100));
    lv_obj_set_height(logo, 120);
    lv_obj_set_style_border_width(logo, 0, 0);
    lv_obj_set_style_radius(logo, 0, 0);
    lv_obj_set_align(logo, LV_ALIGN_CENTER);
    lv_image_set_src(logo, yaiba_logo_black);
    
    lv_obj_t * boot_label = lv_obj_create(lv_obj_0);
    lv_obj_set_name(boot_label, "boot_label");
    lv_obj_set_flex_flow(boot_label, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_cross_place(boot_label, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_main_place(boot_label, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_column(boot_label, 8, 0);
    lv_obj_set_style_bg_opa(boot_label, 0, 0);
    lv_obj_set_style_border_width(boot_label, 0, 0);
    lv_obj_set_width(boot_label, lv_pct(100));
    lv_obj_set_height(boot_label, LV_SIZE_CONTENT);
    lv_obj_set_align(boot_label, LV_ALIGN_CENTER);
    lv_obj_set_y(boot_label, 55);
    lv_obj_set_flag(boot_label, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_t * boot_label_init = lv_label_create(boot_label);
    lv_obj_set_name(boot_label_init, "boot_label_init");
    lv_label_set_text(boot_label_init, "INITIALIZING...");
    lv_obj_set_style_text_color(boot_label_init, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(boot_label_init, crixus_italic_lg, 0);
    
    lv_obj_t * boot_label_spinner = lv_spinner_create(boot_label);
    lv_obj_set_name(boot_label_spinner, "boot_label_spinner");
    lv_obj_set_width(boot_label_spinner, 20);
    lv_obj_set_height(boot_label_spinner, 20);
    lv_spinner_set_anim_duration(boot_label_spinner, 900);
    lv_spinner_set_arc_sweep(boot_label_spinner, 270);
    lv_obj_set_flag(boot_label_spinner, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_add_style(boot_label_spinner, &spinner_main, LV_PART_MAIN);
    lv_obj_add_style(boot_label_spinner, &spinner_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(boot_label_spinner, &spinner_knob, LV_PART_KNOB);
    
    lv_obj_add_screen_create_event(lv_obj_0, LV_EVENT_SCREEN_LOADED, main_create, LV_SCREEN_LOAD_ANIM_FADE_IN, SCREEN_TRANSITION_DURATION, SCREEN_DELAY);
    
    /* create animation timeline(s) */
    lv_anim_timeline_t ** at_array = lv_malloc(sizeof(lv_anim_timeline_t *) * _BOOT_RED_TIMELINE_CNT);
    at_array[BOOT_RED_TIMELINE_BOOT_ANIM] = timeline_boot_anim_create(lv_obj_0);
    lv_obj_set_user_data(lv_obj_0, at_array);
    lv_obj_add_event_cb(lv_obj_0, free_timeline_event_cb, LV_EVENT_DELETE, at_array);

    lv_obj_add_play_timeline_event(lv_obj_0, LV_EVENT_SCREEN_LOAD_START, boot_red_get_timeline(lv_obj_0, BOOT_RED_TIMELINE_BOOT_ANIM), 0, false);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

lv_anim_timeline_t * boot_red_get_timeline(lv_obj_t * obj, boot_red_timeline_t timeline_id)
{
    if (timeline_id >= _BOOT_RED_TIMELINE_CNT) {
        LV_LOG_WARN("boot_red has no timeline with %d ID", timeline_id);
        return NULL;
    }

    lv_anim_timeline_t ** at_array = lv_obj_get_user_data(obj);
    return at_array[timeline_id];
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Helper function to execute animations */
static void int_anim_exec_cb(lv_anim_t * a, int32_t v)
{
    uint32_t data = (lv_uintptr_t)lv_anim_get_user_data(a);
    lv_style_prop_t prop = data >> 24;
    lv_style_selector_t selector = data & 0x00ffffff;

    lv_style_value_t style_value;
    style_value.num = v;
    lv_obj_set_local_style_prop(a->var, prop, style_value, selector);
}

static lv_anim_timeline_t * timeline_boot_anim_create(lv_obj_t * obj)
{
    lv_anim_timeline_t * at = lv_anim_timeline_create();
    lv_anim_timeline_t * at_to_merge = NULL;

    lv_anim_t a;
    uint32_t selector_and_prop;

    selector_and_prop = ((LV_STYLE_OPA & 0xff) << 24) | 0;
    lv_anim_init(&a);
    lv_anim_set_custom_exec_cb(&a, int_anim_exec_cb);
    lv_anim_set_var(&a, lv_obj_find_by_name(obj, "logo_bg"));
    lv_anim_set_values(&a, 0, 255);
    lv_anim_set_duration(&a, LOGO_BG_FADEIN_DURATION);
    lv_anim_set_user_data(&a, (void *)((uintptr_t)selector_and_prop));
    lv_anim_timeline_add(at, 0, &a);

    selector_and_prop = ((LV_STYLE_OPA_LAYERED & 0xff) << 24) | 0;
    lv_anim_init(&a);
    lv_anim_set_custom_exec_cb(&a, int_anim_exec_cb);
    lv_anim_set_var(&a, lv_obj_find_by_name(obj, "logo"));
    lv_anim_set_values(&a, 0, 255);
    lv_anim_set_duration(&a, LOGO_FADEIN_DURATION);
    lv_anim_set_user_data(&a, (void *)((uintptr_t)selector_and_prop));
    lv_anim_set_early_apply(&a, true);
    lv_anim_timeline_add(at, LOGO_DELAY, &a);

    selector_and_prop = ((LV_STYLE_TRANSLATE_X & 0xff) << 24) | 0;
    lv_anim_init(&a);
    lv_anim_set_custom_exec_cb(&a, int_anim_exec_cb);
    lv_anim_set_var(&a, lv_obj_find_by_name(obj, "logo"));
    lv_anim_set_values(&a, 100, 0);
    lv_anim_set_duration(&a, LOGO_FADEIN_DURATION);
    lv_anim_set_user_data(&a, (void *)((uintptr_t)selector_and_prop));
    lv_anim_set_early_apply(&a, true);
    lv_anim_timeline_add(at, LOGO_DELAY, &a);

    selector_and_prop = ((LV_STYLE_OPA & 0xff) << 24) | 0;
    lv_anim_init(&a);
    lv_anim_set_custom_exec_cb(&a, int_anim_exec_cb);
    lv_anim_set_var(&a, lv_obj_find_by_name(obj, "boot_label"));
    lv_anim_set_values(&a, 0, 255);
    lv_anim_set_duration(&a, LABEL_DURATION);
    lv_anim_set_user_data(&a, (void *)((uintptr_t)selector_and_prop));
    lv_anim_set_early_apply(&a, true);
    lv_anim_timeline_add(at, LABEL_DELAY, &a);

    selector_and_prop = ((LV_STYLE_OPA & 0xff) << 24) | 0;
    lv_anim_init(&a);
    lv_anim_set_custom_exec_cb(&a, int_anim_exec_cb);
    lv_anim_set_var(&a, lv_obj_find_by_name(obj, "logo_bg"));
    lv_anim_set_values(&a, 255, 0);
    lv_anim_set_duration(&a, FADEOUT_DURATION);
    lv_anim_set_user_data(&a, (void *)((uintptr_t)selector_and_prop));
    lv_anim_timeline_add(at, FADEOUT_DELAY, &a);

    selector_and_prop = ((LV_STYLE_OPA & 0xff) << 24) | 0;
    lv_anim_init(&a);
    lv_anim_set_custom_exec_cb(&a, int_anim_exec_cb);
    lv_anim_set_var(&a, lv_obj_find_by_name(obj, "logo"));
    lv_anim_set_values(&a, 255, 0);
    lv_anim_set_duration(&a, FADEOUT_DURATION);
    lv_anim_set_user_data(&a, (void *)((uintptr_t)selector_and_prop));
    lv_anim_timeline_add(at, FADEOUT_DELAY, &a);

    selector_and_prop = ((LV_STYLE_TRANSLATE_X & 0xff) << 24) | 0;
    lv_anim_init(&a);
    lv_anim_set_custom_exec_cb(&a, int_anim_exec_cb);
    lv_anim_set_var(&a, lv_obj_find_by_name(obj, "logo"));
    lv_anim_set_values(&a, 0, -100);
    lv_anim_set_duration(&a, FADEOUT_DURATION);
    lv_anim_set_user_data(&a, (void *)((uintptr_t)selector_and_prop));
    lv_anim_timeline_add(at, FADEOUT_DELAY, &a);

    selector_and_prop = ((LV_STYLE_OPA & 0xff) << 24) | 0;
    lv_anim_init(&a);
    lv_anim_set_custom_exec_cb(&a, int_anim_exec_cb);
    lv_anim_set_var(&a, lv_obj_find_by_name(obj, "boot_label"));
    lv_anim_set_values(&a, 255, 0);
    lv_anim_set_duration(&a, FADEOUT_DURATION);
    lv_anim_set_user_data(&a, (void *)((uintptr_t)selector_and_prop));
    lv_anim_timeline_add(at, FADEOUT_DELAY, &a);

    selector_and_prop = ((LV_STYLE_OPA & 0xff) << 24) | 0;
    lv_anim_init(&a);
    lv_anim_set_custom_exec_cb(&a, int_anim_exec_cb);
    lv_anim_set_var(&a, lv_obj_find_by_name(obj, "boot_spinner"));
    lv_anim_set_values(&a, 255, 0);
    lv_anim_set_duration(&a, FADEOUT_DURATION);
    lv_anim_set_user_data(&a, (void *)((uintptr_t)selector_and_prop));
    lv_anim_timeline_add(at, FADEOUT_DELAY, &a);

    return at;
}

static void free_timeline_event_cb(lv_event_t * e)
{
    lv_anim_timeline_t ** at_array = lv_event_get_user_data(e);
    uint32_t i;
    for(i = 0; i < _BOOT_RED_TIMELINE_CNT; i++) {
        lv_anim_timeline_delete(at_array[i]);
    }
    lv_free(at_array);
}

