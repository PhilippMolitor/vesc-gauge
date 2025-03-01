SET(SOURCES ui_screen_boot.c
    ui_screen_main.c
    ui_screen_wled_settings.c
    ui_screen_profile_editor.c
    ui_theme_manager.c
    ui_themes.c
    ui.c
    ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    fonts/ui_font_font_speed_136pt.c
    fonts/ui_font_font_speed_48pt.c)

add_library(ui ${SOURCES})
