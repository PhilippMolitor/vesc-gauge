#include "settings.h"

#include <esp_log.h>

#include <Arduino.h>
#include <Wire.h>

#include <ReactESP.h>

#include <ui.h>

#include "drivers/display/display.h"
#include "drivers/sdcard/sdcard.h"
#include "drivers/tca9554pwr/tca9554.h"
#include "utils/fifo.h"
#include "utils/macros.h"
#include "utils/wmavg.h"
#include "wled_esp_now/wled_esp_now.h"

using namespace reactesp;

static constexpr char* LOG_TAG = "main";

// handles
static EventLoop evloop;

// service state
static FastFIFO<wled_wizmote_cmd, 16> state_wled_cmd;

// ui state
static WeightedMovingAverage<1000, 100> speed;

// ui event hooks
void ui_cb_wled_switch(lv_event_t* e)
{
  state_wled_cmd.enqueue(
      lv_obj_has_state(lv_event_get_target_obj(e), LV_STATE_CHECKED)
          ? wled_wizmote_cmd::POWER_ON
          : wled_wizmote_cmd::POWER_OFF);
}

void ui_cb_wled_brightness_set(lv_event_t* e)
{
  auto obj = lv_event_get_target_obj(e);
  auto value = lv_slider_get_value(obj);
  auto min = lv_slider_get_min_value(obj);
  auto max = lv_slider_get_max_value(obj);

  uint8_t offset = map(value, min, max, 0, WLED_ESP_NOW_COUNT_BRIGHTNESS - 1);
  state_wled_cmd.enqueue(static_cast<wled_wizmote_cmd>(wled_wizmote_cmd::BRIGHTNESS_1 + offset));
}

// tasks

void task_vesc_poll()
{
  // TODO: poll actual VESC data

  static uint32_t startTime = millis();
  uint32_t currentTime = millis();
  float elapsedTime = (currentTime - startTime) / 1000.0;
  if (elapsedTime >= 4.0)
    startTime = currentTime; // reset after 4 seconds

  float value = 25.0 + 25.0 * sin((PI / 2.0) * elapsedTime);
  speed.update(value);
}

void task_vesc_uidata()
{
  // ui data cache
  static float cache_speed = 0;

  // gauge
  auto fresh_speed = speed.filteredValue();
  if (!FLOAT_COMPARE_E(fresh_speed, cache_speed, 0.4)) {
    // update cache
    cache_speed = fresh_speed;
    // update UI
    char speed_str[10];
    snprintf(speed_str, sizeof(speed_str), "%.0f", fresh_speed);
    lv_label_set_text(ui_main_label_speed_value, speed_str);
  }
}

void task_wled()
{
  static bool initialized = false;

  if (!initialized) {
    // set esp-now mac address in UI
    uint8_t wled_mac[6];
    wled_esp_now_mac_get(wled_mac);
    char wled_mac_str[23];
    snprintf(wled_mac_str, sizeof(wled_mac_str), "MAC: %02X:%02X:%02X:%02X:%02X:%02X",
        wled_mac[0], wled_mac[1], wled_mac[2], wled_mac[3], wled_mac[4], wled_mac[5]);
    lv_label_set_text(ui_wled_settings_label_macaddr, wled_mac_str);

    initialized = true;
  }

  wled_wizmote_cmd cmd;
  if (state_wled_cmd.dequeue(&cmd))
    wled_esp_now_wizmote_cmd(static_cast<wled_wizmote_cmd>(cmd));
}

void task_debug_perfmon()
{
  ESP_LOGD(LOG_TAG, "heap: %.1fkB free of %.1fkB",
      ESP.getFreeHeap() / 1024.0,
      ESP.getHeapSize() / 1024.0);
  ESP_LOGD(LOG_TAG, "psram: %.1fkB free of %.1fkB",
      ESP.getFreePsram() / 1024.0,
      ESP.getPsramSize() / 1024.0);
}

void setup()
{
  analogReadResolution(ANALOG_READ_RESOLUTION);
  Wire.begin(SDA, SCL);
  Serial.begin(115200);

  // IO expander
  tca9554pwr_init(0x00);
  // turn off buzzer
  tca9554pwr_write(PIN_TCA9554PWR_BUZZER, LOW);

  // important: this must be done before initializing the display,
  // otherwise the microcontroller will crash.
  wled_esp_now_init();

  display_init();
  ui_init();

  // tasks
  evloop.onRepeat(Hz(200), lv_task_handler);
  evloop.onRepeat(Hz(10), task_vesc_poll);
  evloop.onRepeat(Hz(16), task_vesc_uidata);
  evloop.onRepeat(Hz(8), task_wled);

  // debugging
  evloop.onRepeat(Hz(0.1), task_debug_perfmon);
}

void loop()
{
  evloop.tick();
}
