#pragma once

#include <inttypes.h>

#define CST820_LCD_TOUCH_MAX_POINTS (1)

enum class cst820_gesture : uint8_t {
  NONE = 0x00,
  SWIPE_UP = 0x01,
  SWIPE_DOWN = 0x02,
  SWIPE_LEFT = 0x03,
  SWIPE_RIGHT = 0x04,
  SINGLE_CLICK = 0x05,
  DOUBLE_CLICK = 0x0B,
  LONG_PRESS = 0x0C
};

typedef struct cst820_event {
  uint8_t points;
  cst820_gesture gesture;
  uint16_t x;
  uint16_t y;
} cst820_event;

uint8_t cst820_init();
uint8_t cst820_read(cst820_event* data);
