#include <Arduino.h>

#include "drivers/display/display.h"
#include "drivers/sdcard/sdcard.h"
#include "ui.h"

void peripherals_init()
{
  Wire.begin(15, 7);
  // Wire.begin(SDA, SCL);

  // IO expander
  TCA9554PWR_Init(0x00);
  Set_EXIO(EXIO_PIN8, EXIO_LOW);
}

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
  Serial.begin(115200);

  peripherals_init();
  SD_Init();

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
