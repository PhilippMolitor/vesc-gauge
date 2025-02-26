#pragma once

#include <Wire.h>

bool i2c_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len);
bool i2c_write(uint8_t addr, uint8_t reg, const uint8_t* data, uint32_t len);
