#include "settings.h"

#include <esp_log.h>

#include <Arduino.h>

#include <ComEVesc.h>
#include <ReactESP.h>

#include <ui.h>

#include "drivers/display/display.h"
#include "drivers/sdcard/sdcard.h"
#include "drivers/tca9554pwr/tca9554pwr.h"
#include "utils/timing.h"
#include "utils/wmavg.h"
#include "wled_esp_now/wled_esp_now.h"

using namespace reactesp;

static constexpr char* LOG_TAG = "main";

// handles
static EventLoop evloop;
static ComEVesc vesc(50);

// service state
static bool state_wled_power_on = false;
static bool state_wled_brightness_dec = false;
static bool state_wled_brightness_inc = false;

// ui state
static WeightedMovingAverage<1000, 100> speed;
static uint8_t wled_mac[6] = { 0 };

void ui_cb_wled_switch(lv_event_t* e)
{
  lv_obj_t* obj = lv_event_get_target_obj(e);
  auto state = lv_obj_has_state(obj, LV_STATE_CHECKED);
  state_wled_power_on = state;
}

void ui_cb_wled_brightness_dec(lv_event_t* e)
{
  state_wled_brightness_dec = true;
}

void ui_cb_wled_brightness_inc(lv_event_t* e)
{
  state_wled_brightness_inc = true;
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

void task_vesc_poll()
{
  // TODO: enable
  // vesc.getVescValues();

  static uint32_t startTime = millis();
  uint32_t currentTime = millis();
  float elapsedTime = (currentTime - startTime) / 1000.0;
  if (elapsedTime >= 4.0)
    startTime = currentTime; // reset after 4 seconds

  float value = 25.0 + 25.0 * sin((PI / 2.0) * elapsedTime);
  speed.update(value);
}

void task_ui_data_refresh()
{
  // gauge
  auto speed_value = speed.filteredValue();
  char speed_str[10];
  snprintf(speed_str, sizeof(speed_str), "%.0f", speed_value);

  lv_label_set_text(ui_main_label_speed_value, speed_str);

  // wled
  lv_label_set_text(ui_wled_settings_label_status, state_wled_power_on ? "active" : "inactive");
  char wled_mac_str[23];
  snprintf(wled_mac_str, sizeof(wled_mac_str), "MAC: %02X:%02X:%02X:%02X:%02X:%02X",
      wled_mac[0], wled_mac[1], wled_mac[2], wled_mac[3], wled_mac[4], wled_mac[5]);
  lv_label_set_text(ui_wled_settings_label_macaddr, wled_mac_str);
}

void task_wled()
{
  static bool initialized = false;

  static bool last_power_on = state_wled_power_on;

  // initialize esp_now
  if (!initialized) {
    wled_esp_now_mac_get(wled_mac);
    initialized = true;
    return;
  }

  // switch power
  if (last_power_on != state_wled_power_on) {
    wled_esp_now_send(state_wled_power_on ? wled_esp_now_cmd::POWER_ON : wled_esp_now_cmd::POWER_OFF);
    last_power_on = state_wled_power_on;
    return;
  }

  // brightness decrease
  if (state_wled_brightness_dec) {
    wled_esp_now_send(wled_esp_now_cmd::BRIGHTNESS_DOWN);
    state_wled_brightness_dec = false;
  }

  // brightness increase
  if (state_wled_brightness_inc) {
    wled_esp_now_send(wled_esp_now_cmd::BRIGHTNESS_UP);
    state_wled_brightness_inc = false;
  }
}

void setup()
{
  analogReadResolution(ANALOG_READ_RESOLUTION);
  Wire.begin(SDA, SCL);

  // IO expander
  tca9554pwr_init(0x00);
  // turn off buzzer
  tca9554pwr_write(PIN_TCA9554PWR_BUZZER, LOW);

  // important: this must be done before initializing the display,
  // otherwise the microcontroller will crash.
  wled_esp_now_init(1);

  display_init();
  ui_init();

  Serial.begin(115200);
  vesc.setSerialPort(&Serial);

  // system
  evloop.onRepeat(Hz(250), lv_task_handler);
  evloop.onRepeat(Hz(16), task_ui_data_refresh);

  // services
  evloop.onRepeat(Hz(10), task_vesc_poll);
  evloop.onRepeat(Hz(4), task_wled);

  // debug log task
  evloop.onRepeat(Hz(0.1), task_debug_perfmon);
}

void loop()
{
  evloop.tick();
}
