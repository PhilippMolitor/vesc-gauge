/**
 * @file ui_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "ui_gen.h"

#if LV_USE_XML
#endif /* LV_USE_XML */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/*----------------
 * Translations
 *----------------*/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*--------------------
 *  Permanent screens
 *-------------------*/

/*----------------
 * Fonts
 *----------------*/

lv_font_t * manrope_md;
extern uint8_t manrope_regular_ttf_data[];
extern size_t manrope_regular_ttf_data_size;
lv_font_t * manrope_bold_md;
extern uint8_t manrope_bold_ttf_data[];
extern size_t manrope_bold_ttf_data_size;
lv_font_t * crixus_italic_xxl;
extern uint8_t crixus_expanded_italic_ttf_data[];
extern size_t crixus_expanded_italic_ttf_data_size;
lv_font_t * crixus_italic_3xl;

/*----------------
 * Images
 *----------------*/

/*----------------
 * Global styles
 *----------------*/

/*----------------
 * Subjects
 *----------------*/

lv_subject_t bat_voltage;
lv_subject_t bat_cells;
lv_subject_t bat_cell_voltage_min;
lv_subject_t bat_cell_voltage_max;
lv_subject_t bat_cell_voltage_diff;
lv_subject_t bat_soc;
lv_subject_t speed_ms;
lv_subject_t esc_power_w;
lv_subject_t periph_headlight;
lv_subject_t periph_taillight;
lv_subject_t periph_breaklight;
lv_subject_t periph_turnsignal;
lv_subject_t periph_lights_enabled;
lv_subject_t periph_breaklight_enabled;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ui_init_gen(const char * asset_path)
{
    char buf[256];


    /*----------------
     * Fonts
     *----------------*/

    /* create tiny ttf font 'manrope_md' from C array */
    manrope_md = lv_tiny_ttf_create_data(manrope_regular_ttf_data, manrope_regular_ttf_data_size, 24);
    /* create tiny ttf font 'manrope_bold_md' from C array */
    manrope_bold_md = lv_tiny_ttf_create_data(manrope_bold_ttf_data, manrope_bold_ttf_data_size, 24);
    /* create tiny ttf font 'crixus_italic_xxl' from C array */
    crixus_italic_xxl = lv_tiny_ttf_create_data(crixus_expanded_italic_ttf_data, crixus_expanded_italic_ttf_data_size, 48);
    /* create tiny ttf font 'crixus_italic_3xl' from C array */
    crixus_italic_3xl = lv_tiny_ttf_create_data(crixus_expanded_italic_ttf_data, crixus_expanded_italic_ttf_data_size, 148);


    /*----------------
     * Images
     *----------------*/
    /*----------------
     * Global styles
     *----------------*/

    /*----------------
     * Subjects
     *----------------*/
    lv_subject_init_float(&bat_voltage, 0);
    lv_subject_init_float(&bat_cells, 0);
    lv_subject_init_float(&bat_cell_voltage_min, 0);
    lv_subject_init_float(&bat_cell_voltage_max, 0);
    lv_subject_init_float(&bat_cell_voltage_diff, 0);
    lv_subject_init_int(&bat_soc, 0);
    lv_subject_set_min_value_int(&bat_soc, 0);
    lv_subject_set_max_value_int(&bat_soc, 100);
    lv_subject_init_float(&speed_ms, 0);
    lv_subject_init_float(&esc_power_w, 0);
    lv_subject_init_int(&periph_headlight, 0);
    lv_subject_set_min_value_int(&periph_headlight, 0);
    lv_subject_set_max_value_int(&periph_headlight, 1);
    lv_subject_init_int(&periph_taillight, 0);
    lv_subject_set_min_value_int(&periph_taillight, 0);
    lv_subject_set_max_value_int(&periph_taillight, 1);
    lv_subject_init_int(&periph_breaklight, 0);
    lv_subject_set_min_value_int(&periph_breaklight, 0);
    lv_subject_set_max_value_int(&periph_breaklight, 1);
    lv_subject_init_int(&periph_turnsignal, 0);
    lv_subject_set_min_value_int(&periph_turnsignal, -1);
    lv_subject_set_max_value_int(&periph_turnsignal, 1);
    lv_subject_init_int(&periph_lights_enabled, 0);
    lv_subject_set_min_value_int(&periph_lights_enabled, 0);
    lv_subject_set_max_value_int(&periph_lights_enabled, 1);
    lv_subject_init_int(&periph_breaklight_enabled, 0);
    lv_subject_set_min_value_int(&periph_breaklight_enabled, 0);
    lv_subject_set_max_value_int(&periph_breaklight_enabled, 1);

    /*----------------
     * Translations
     *----------------*/

#if LV_USE_XML
    /* Register widgets */

    /* Register fonts */
    lv_xml_register_font(NULL, "manrope_md", manrope_md);
    lv_xml_register_font(NULL, "manrope_bold_md", manrope_bold_md);
    lv_xml_register_font(NULL, "crixus_italic_xxl", crixus_italic_xxl);
    lv_xml_register_font(NULL, "crixus_italic_3xl", crixus_italic_3xl);

    /* Register subjects */
    lv_xml_register_subject(NULL, "bat_voltage", &bat_voltage);
    lv_xml_register_subject(NULL, "bat_cells", &bat_cells);
    lv_xml_register_subject(NULL, "bat_cell_voltage_min", &bat_cell_voltage_min);
    lv_xml_register_subject(NULL, "bat_cell_voltage_max", &bat_cell_voltage_max);
    lv_xml_register_subject(NULL, "bat_cell_voltage_diff", &bat_cell_voltage_diff);
    lv_xml_register_subject(NULL, "bat_soc", &bat_soc);
    lv_xml_register_subject(NULL, "speed_ms", &speed_ms);
    lv_xml_register_subject(NULL, "esc_power_w", &esc_power_w);
    lv_xml_register_subject(NULL, "periph_headlight", &periph_headlight);
    lv_xml_register_subject(NULL, "periph_taillight", &periph_taillight);
    lv_xml_register_subject(NULL, "periph_breaklight", &periph_breaklight);
    lv_xml_register_subject(NULL, "periph_turnsignal", &periph_turnsignal);
    lv_xml_register_subject(NULL, "periph_lights_enabled", &periph_lights_enabled);
    lv_xml_register_subject(NULL, "periph_breaklight_enabled", &periph_breaklight_enabled);

    /* Register callbacks */
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
#endif

#if LV_USE_XML == 0
    /*--------------------
     *  Permanent screens
     *-------------------*/
    /* If XML is enabled it's assumed that the permanent screens are created
     * manaully from XML using lv_xml_create() */
#endif
}

/* Callbacks */

/**********************
 *   STATIC FUNCTIONS
 **********************/