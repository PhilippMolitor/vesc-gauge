#pragma once

#include <Arduino.h>

#define BATTERY_ADC_OFFSET 0.992857

void battery_get_voltage(float* out);
