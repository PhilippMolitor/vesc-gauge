#include "display.h"

lv_disp_drv_t disp_drv;

static lv_disp_draw_buf_t draw_buf;
IRAM_ATTR static void* buf1 = NULL;
IRAM_ATTR static void* buf2 = NULL;
// static lv_color_t buf1[ LVGL_BUF_LEN ];
// static lv_color_t buf2[ LVGL_BUF_LEN ];
// static lv_color_t* buf1 = (lv_color_t*) heap_caps_malloc(LVGL_BUF_LEN, MALLOC_CAP_SPIRAM);
// static lv_color_t* buf2 = (lv_color_t*) heap_caps_malloc(LVGL_BUF_LEN, MALLOC_CAP_SPIRAM);

void display_flush(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p)
{
  st7701_draw_rect(area->x1, area->y1, area->x2, area->y2, (uint8_t*)&color_p->full);
  lv_disp_flush_ready(disp_drv);
}

void display_touchpad_update(lv_indev_drv_t* indev_drv, lv_indev_data_t* data)
{
  cst820_event inputs;
  touch_read(&inputs);

  if (inputs.points != 0x00) {
    data->point.x = inputs.x;
    data->point.y = inputs.y;
    data->state = LV_INDEV_STATE_PR;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }

  // Gestures are detected by the input points over time
  // if (inputs.gesture != NONE) {}
}

void display_tick(void* arg)
{
  lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

void display_init(void)
{
  st7701_setup();

  lv_init();
  esp_lcd_rgb_panel_get_frame_buffer(panel, 2, &buf1, &buf2);
  lv_disp_draw_buf_init(
      &draw_buf,
      buf1,
      buf2,
      ESP_PANEL_LCD_WIDTH * ESP_PANEL_LCD_HEIGHT);

  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LCD_HEIGHT;
  disp_drv.ver_res = LCD_WIDTH;
  disp_drv.flush_cb = display_flush;
  disp_drv.full_refresh = 1;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = display_touchpad_update;
  lv_indev_drv_register(&indev_drv);

  const esp_timer_create_args_t lvgl_tick_timer_args = {
    .callback = &display_tick,
    .name = "lvgl_tick"
  };
  esp_timer_handle_t lvgl_tick_timer = NULL;
  esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
  esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000);
}
