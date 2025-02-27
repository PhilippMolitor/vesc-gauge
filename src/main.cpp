#include "settings.h"

#include <Arduino.h>

#include "drivers/display/display.h"
#include "drivers/sdcard/sdcard.h"
#include "drivers/tca9554pwr/tca9554pwr.h"
#include "ui.h"

void ui_update_data()
{
  static uint32_t startTime = millis();
  uint32_t currentTime = millis();
  float elapsedTime = (currentTime - startTime) / 1000.0;

  float value = 25.0 + 25.0 * sin((PI / 2.0) * elapsedTime);

  if (elapsedTime >= 4.0)
    startTime = currentTime; // reset after 4 seconds

  char buffer[10];
  snprintf(buffer, sizeof(buffer), "%.0f", value);
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
}

void loop()
{
  static uint32_t lastUpdateTime = 0;
  uint32_t currentTime = millis();
  if (currentTime - lastUpdateTime >= 200) {
    ui_update_data();
    lastUpdateTime = currentTime;
  }

  lv_task_handler();
  vTaskDelay(pdMS_TO_TICKS(5));
}
