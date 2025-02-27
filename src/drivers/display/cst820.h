#pragma once

#include <Arduino.h>

#include "../../utils/i2c.h"
#include "../tca9554pwr/tca9554pwr.h"

#define CST820_ADDR (0x15)

#define CST820_REG_MODE 0x00
#define CST820_REG_GESTURE_ID 0x01
#define CST820_REG_VERSION 0x15
#define CST820_REG_CHIPID 0xA7
#define CST820_REG_PROJID 0xA8
#define CST820_REG_FW_VERSION 0xA9
#define CST820_REG_AUTOSLEEP_TIME 0xF9
#define CST820_REG_AUTOSLEEP_DISABLE 0xFE

#define CST820_LCD_TOUCH_MAX_POINTS (1)

enum cst820_gesture : uint8_t {
  NONE = 0x00,
  SWIPE_UP = 0x01,
  SWIPE_DOWN = 0x02,
  SWIPE_LEFT = 0x03,
  SWIPE_RIGHT = 0x04,
  SINGLE_CLICK = 0x05,
  DOUBLE_CLICK = 0x0B,
  LONG_PRESS = 0x0C
};

struct cst820_event {
  uint8_t points;
  cst820_gesture gesture;
  uint16_t x;
  uint16_t y;
};

uint8_t touch_init();
uint8_t touch_read(cst820_event* data);
