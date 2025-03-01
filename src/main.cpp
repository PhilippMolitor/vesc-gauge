#include "settings.h"

#include <esp_log.h>

#include <Arduino.h>

#include <ComEVesc.h>
#include <ReactESP.h>

#include <ui.h>

#include "drivers/display/display.h"
#include "drivers/sdcard/sdcard.h"
#include "drivers/tca9554pwr/tca9554pwr.h"
#include "utils/lpfv.h"
#include "wled_esp_now/wled_esp_now.h"

using namespace reactesp;

static constexpr char* LOG_TAG = "main";

// handles
static EventLoop evloop;
static ComEVesc vesc(TIMEOUT_VESC_READ);

// service state
static bool state_wled_enabled = false;
static bool state_wled_power_on = false;

// ui state
static TimeBasedLPF speed;
static uint8_t wled_mac[6] = { 0 };

void ui_cb_wled_switch(lv_event_t* e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t* obj = lv_event_get_target_obj(e);
  if (code == LV_EVENT_VALUE_CHANGED) {
    auto state = lv_obj_has_state(obj, LV_STATE_CHECKED);
    state_wled_power_on = state;
    LV_UNUSED(obj);
    ESP_LOGI(LOG_TAG, "wled switch: %s", state ? "on" : "off");
  }
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
  auto speed_value = speed.value();
  char speed_str[10];
  snprintf(speed_str, sizeof(speed_str), "%.0f", speed_value);

  lv_label_set_text(ui_main_label_speed_value, speed_str);

  // wled
  lv_label_set_text(ui_wled_settings_label_status, state_wled_enabled ? "active" : "inactive");
  char wled_mac_str[23];
  snprintf(wled_mac_str, sizeof(wled_mac_str), "MAC: %02X:%02X:%02X:%02X:%02X:%02X",
      wled_mac[0], wled_mac[1], wled_mac[2], wled_mac[3], wled_mac[4], wled_mac[5]);
  lv_label_set_text(ui_wled_settings_label_macaddr, wled_mac_str);
}

void task_wled()
{
  static bool initialized = false;

  static bool last_enabled = state_wled_enabled;
  static bool last_power_on = state_wled_power_on;

  // initialize esp_now
  if (!initialized) {
    wled_esp_now_init(1);
    wled_esp_now_mac_get(wled_mac);
    initialized = true;

    ESP_LOGI(LOG_TAG, "initialized wled!");
    return;
  }

  // TODO: deinitialize
  if (false) {
    wled_esp_now_deinit();
    initialized = false;
  }

  // switch power
  if (last_power_on != state_wled_power_on) {
    wled_esp_now_send(state_wled_power_on ? wled_esp_now_cmd::POWER_ON : wled_esp_now_cmd::POWER_OFF);
    last_power_on = state_wled_power_on;

    ESP_LOGI(LOG_TAG, "wled power: %s", state_wled_power_on ? "on" : "off");
    return;
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

  display_init();
  ui_init();

  Serial.begin(115200);
  vesc.setSerialPort(&Serial);

  // system
  evloop.onRepeat(2, lv_task_handler);
  evloop.onRepeat(1000 / UPDATE_HZ_UI_DATA, task_ui_data_refresh);

  // services
  evloop.onRepeat(1000 / UPDATE_HZ_VESC_POLL, task_vesc_poll);
  evloop.onRepeat(1000, task_wled);

  // debug log task
  evloop.onRepeat(5000, task_debug_perfmon);
}

void loop()
{
  evloop.tick();
}
