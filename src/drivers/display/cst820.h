#pragma once

#include <Arduino.h>

#include "../../utils/i2c.h"
#include "../ioexpander/TCA9554PWR.h"

#define CST820_ADDR 0x15
#define CST820_INT_PIN 16
#define CST820_LCD_TOUCH_MAX_POINTS (1)

#define CST820_REG_Mode 0x00
#define CST820_REG_GestureID 0x01
#define CST820_REG_Version 0x15
#define CST820_REG_ChipID 0xA7
#define CST820_REG_ProjID 0xA8
#define CST820_REG_FwVersion 0xA9
#define CST820_REG_AutoSleepTime 0xF9
#define CST820_REG_DisAutoSleep 0xFE

// Interrupt Modes
#define RISING 0x01
#define FALLING 0x02
#define CHANGE 0x03
#define ONLOW 0x04
#define ONHIGH 0x05
#define ONLOW_WE 0x0C
#define ONHIGH_WE 0x0D

#define interrupt RISING

/* CST820 GESTURE */
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

static bool cst820_reset(void);
static bool cst820_config_read(void);
static void cst820_autosleep_set(bool enable);
uint8_t touch_init();
uint8_t touch_read(cst820_event* data);
