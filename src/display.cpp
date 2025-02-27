#include "display.h"

static lv_color_t* frame_buf_a = (lv_color_t*)heap_caps_aligned_alloc(32, FB_SIZE * 2, MALLOC_CAP_DMA);
static lv_color_t* frame_buf_b = (lv_color_t*)heap_caps_aligned_alloc(32, FB_SIZE * 2, MALLOC_CAP_DMA);

void display_flush(lv_display_t* instance, const lv_area_t* area, uint8_t* px_map)
{
  st7701_draw_rect(area->x1, area->y1, area->x2, area->y2, px_map);
  lv_display_flush_ready(instance);
}

void display_touchpad_update(lv_indev_t* drv, lv_indev_data_t* data)
{
  cst820_event inputs;
  touch_read(&inputs);

  if (inputs.points != 0x00) {
    data->point.x = inputs.x;
    data->point.y = inputs.y;
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }

  // Gestures are detected by the input points over time
  // if (inputs.gesture != NONE) {}
}

void display_init(void)
{
  st7701_setup(); // configure the display first

  lv_init();

  // lvgl tick
  lv_tick_set_cb(xTaskGetTickCount);

  // initialize display/buffer
  lv_display_t* display = lv_display_create(ESP_PANEL_LCD_WIDTH, ESP_PANEL_LCD_HEIGHT);
  lv_display_set_flush_cb(display, display_flush);
  lv_display_set_buffers(
      display,
      frame_buf_a, frame_buf_b, FB_SIZE,
      LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_resolution(display, ESP_PANEL_LCD_WIDTH, ESP_PANEL_LCD_HEIGHT);
  lv_display_set_physical_resolution(display, ESP_PANEL_LCD_WIDTH, ESP_PANEL_LCD_HEIGHT);

  // initialize display driver
  static lv_indev_t* indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, display_touchpad_update);
}
