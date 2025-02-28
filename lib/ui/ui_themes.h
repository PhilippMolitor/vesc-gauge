// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.0
// LVGL version: 9.1.0
// Project name: pocketbikedisplay

#ifndef _UI_THEMES_H
#define _UI_THEMES_H

#ifdef __cplusplus
extern "C" {
#endif

#define UI_THEME_COLOR_BACKGROUND 0

#define UI_THEME_DEFAULT 0

#define UI_THEME_DARK 1

#define UI_THEME_LIGHT 2

extern const ui_theme_variable_t _ui_theme_color_background[3];
extern const ui_theme_variable_t _ui_theme_alpha_background[3];

extern const uint32_t* ui_theme_colors[3];
extern const uint8_t* ui_theme_alphas[3];
extern uint8_t ui_theme_idx;

void ui_theme_set(uint8_t theme_idx);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
