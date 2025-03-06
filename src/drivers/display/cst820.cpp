#include "cst820.h"

#include <Arduino.h>

#include "../../utils/i2c.h"
#include "../tca9554pwr/tca9554.h"

#define CST820_ADDR (0x15)

#define CST820_REG_MODE 0x00
#define CST820_REG_GESTURE_ID 0x01
#define CST820_REG_VERSION 0x15
#define CST820_REG_CHIPID 0xA7
#define CST820_REG_PROJID 0xA8
#define CST820_REG_FW_VERSION 0xA9
#define CST820_REG_AUTOSLEEP_TIME 0xF9
#define CST820_REG_AUTOSLEEP_DISABLE 0xFE

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

uint8_t cst820_init()
{
  cst820_reset();
  cst820_autosleep_set(false);

  return true;
}

uint8_t cst820_read(cst820_event* data)
{
  uint8_t buf[6];
  i2c_read(CST820_ADDR, CST820_REG_GESTURE_ID, buf, 6);

  noInterrupts();
  {
    // gesture detected
    if (buf[0] != 0x00) {
      data->gesture = (cst820_gesture)buf[0];
    }

    // point touch event
    if (buf[1] != 0x00) {
      data->points = (uint8_t)buf[1];
      if (data->points < CST820_LCD_TOUCH_MAX_POINTS + 1) {
        data->x = ((buf[2] & 0x0F) << 8) + buf[3];
        data->y = ((buf[4] & 0x0F) << 8) + buf[5];
      }
    }
  }
  interrupts();

  return true;
}
