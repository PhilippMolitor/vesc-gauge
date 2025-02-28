#include "settings.h"

#include <Arduino.h>
#include <ReactESP.h>

#include "drivers/display/display.h"
#include "drivers/sdcard/sdcard.h"
#include "drivers/tca9554pwr/tca9554pwr.h"
#include "ui.h"
#include "utils/lpfv.h"

using namespace reactesp;

static EventLoop evloop;
static TimeBasedLPF speed;

void task_vesc_poll()
{
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
  char buffer[10];
  snprintf(buffer, sizeof(buffer), "%.0f", speed.value());
  lv_label_set_text(ui_main_label_speed_value, buffer);
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

  evloop.onRepeat(2, lv_task_handler);
  evloop.onRepeat(1000 / UPDATE_HZ_VESC_POLL, task_vesc_poll);
  evloop.onRepeat(1000 / UPDATE_HZ_UI_DATA, task_ui_data_refresh);
}

void loop()
{
  evloop.tick();
}
