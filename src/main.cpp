#include <Arduino.h>

#include "drivers/imu/Gyro_QMI8658.h"
#include "drivers/rtc/RTC_PCF85063.h"
#include "drivers/sdcard/sdcard.h"
#include "utils/Wireless.h"

#include "display.h"
#include "ui.h"

void task_peripherals(void* parameter)
{
  for (;;) {
    QMI8658_Loop();
    RTC_Loop();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void peripherals_init()
{
  Wire.begin(15, 7);
  // Wire.begin(SDA, SCL);

  // IO expander
  TCA9554PWR_Init(0x00);
  Set_EXIO(EXIO_PIN8, EXIO_LOW);

  PCF85063_Init();
  QMI8658_Init();
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

  // periodical task for peripherals
  xTaskCreatePinnedToCore(
      task_peripherals,
      "peripherals",
      4096,
      NULL,
      3,
      NULL,
      0);
}

void loop()
{
  ui_update_data();
  lv_task_handler();
  vTaskDelay(pdMS_TO_TICKS(5));
}
