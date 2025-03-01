#include "cst820.h"

static bool cst820_reset(void)
{
  tca9554pwr_write(PIN_TCA9554PWR_TOUCH_RESET, LOW);
  vTaskDelay(pdMS_TO_TICKS(10));
  tca9554pwr_write(PIN_TCA9554PWR_TOUCH_RESET, HIGH);
  vTaskDelay(pdMS_TO_TICKS(50));

  return true;
}

static void cst820_autosleep_set(bool enable)
{
  cst820_reset();
  uint8_t state = enable ? 0x00 : 0xFF;
  i2c_write(CST820_ADDR, CST820_REG_AUTOSLEEP_DISABLE, &state, 1);
}

uint8_t cst820_init(void)
{
  pinMode(PIN_TOUCH_INTERRUPT, INPUT_PULLUP);

  cst820_reset();
  cst820_autosleep_set(false);

  return true;
}

uint8_t cst820_read(cst820_event* data)
{
  uint8_t buf[6];
  i2c_read(CST820_ADDR, CST820_REG_GESTURE_ID, buf, 6);

  // gesture detected
  if (buf[0] != 0x00)
    data->gesture = (cst820_gesture)buf[0];

  // point touch event
  if (buf[1] != 0x00) {
    noInterrupts();
    data->points = (uint8_t)buf[1];
    if (data->points > CST820_LCD_TOUCH_MAX_POINTS)
      data->points = CST820_LCD_TOUCH_MAX_POINTS;

    data->x = ((buf[2] & 0x0F) << 8) + buf[3];
    data->y = ((buf[4] & 0x0F) << 8) + buf[5];

    interrupts();
  }

  return true;
}
