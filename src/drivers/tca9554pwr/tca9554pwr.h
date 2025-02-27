#pragma once

#include <stdio.h>

#include <esp_log.h>

#include <Wire.h>

#define TCA9554PWR_ADDRESS (0x20)

#define TCA9554PWR_REG_INPUT (0x00)
#define TCA9554PWR_REG_OUTPUT (0x01)
#define TCA9554PWR_REG_POLARITY (0x02)
#define TCA9554PWR_REG_CONFIG (0x03)

void tca9554pwr_init(uint8_t state = 0x00);
void tca9554pwr_configure(uint8_t pin, uint8_t state);
void tca9554pwr_configure_all(uint8_t state);
uint8_t tca9554pwr_read(uint8_t pin);
uint8_t tca9554pwr_read_all(uint8_t reg);
void tca9554pwr_write(uint8_t pin, uint8_t state);
void tca9554pwr_write_all(uint8_t state);
void tca9554pwr_toggle(uint8_t pin);
