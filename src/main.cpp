#include "settings.h"

#include <esp_log.h>

#include <Arduino.h>

#include <ReactESP.h>

#include <ui.h>

#include "drivers/display/display.h"
#include "drivers/sdcard/sdcard.h"
#include "drivers/tca9554pwr/tca9554pwr.h"
#include "utils/fifo.h"
#include "utils/macros.h"
#include "utils/wmavg.h"
#include "wled_esp_now/wled_esp_now.h"

using namespace reactesp;

static constexpr char* LOG_TAG = "main";

// handles
static EventLoop evloop;

// service state
typedef struct {
  uint8_t cmd; // 0x00 = wled_esp_now_cmd, 0x01 = set channel
  uint8_t arg;
} cmd_ui_wled;
static FastFIFO<cmd_ui_wled, 4> state_wled_cmd;

// ui state
static WeightedMovingAverage<1000, 100> speed;
static uint8_t wled_mac[6] = { 0 };

// ui event hooks
void ui_cb_wled_switch(lv_event_t* e)
{
  state_wled_cmd.enqueue(
      (lv_event_get_target_obj(e), LV_STATE_CHECKED)
          ? (cmd_ui_wled) { .cmd = 0, .arg = wled_esp_now_cmd::POWER_ON }
          : (cmd_ui_wled) { .cmd = 0, .arg = wled_esp_now_cmd::POWER_OFF });
}
void ui_cb_wled_brightness_dec(lv_event_t* e) { state_wled_cmd.enqueue((cmd_ui_wled) { .cmd = 0, .arg = wled_esp_now_cmd::BRIGHTNESS_DOWN }); }
void ui_cb_wled_brightness_inc(lv_event_t* e) { state_wled_cmd.enqueue((cmd_ui_wled) { .cmd = 0, .arg = wled_esp_now_cmd::BRIGHTNESS_UP }); }

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
  // ui data cache
  static float cache_speed = 0;

  // gauge
  auto fresh_speed = speed.filteredValue();
  if (!FLOAT_COMPARE(fresh_speed, cache_speed)) {
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
    char wled_mac_str[23];
    snprintf(wled_mac_str, sizeof(wled_mac_str), "MAC: %02X:%02X:%02X:%02X:%02X:%02X",
        wled_mac[0], wled_mac[1], wled_mac[2], wled_mac[3], wled_mac[4], wled_mac[5]);
    lv_label_set_text(ui_wled_settings_label_macaddr, wled_mac_str);

    initialized = true;
  }

  cmd_ui_wled cmd;
  if (!state_wled_cmd.dequeue(&cmd))
    return;

  switch (cmd.cmd) {
  case 0x00:
    switch (cmd.arg) {
    case wled_esp_now_cmd::POWER_ON:
      lv_label_set_text(ui_wled_settings_label_status, "on");
      break;
    case wled_esp_now_cmd::POWER_OFF:
      lv_label_set_text(ui_wled_settings_label_status, "off");
      break;
    case wled_esp_now_cmd::BRIGHTNESS_DOWN:
    case wled_esp_now_cmd::BRIGHTNESS_UP:
      break;
    };
    wled_esp_now_send(static_cast<wled_esp_now_cmd>(cmd.arg));
    break;

  case 0x01:
    wled_esp_now_channel_set(cmd.arg);
    break;

  default:
    break;
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
  wled_esp_now_init();

  display_init();
  ui_init();

  Serial.begin(115200);

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
