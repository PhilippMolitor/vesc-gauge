#include "battery.h"

void battery_get_voltage(float* out)
{
  int adc_voltage = analogReadMilliVolts(PIN_BATTERY_ADC);
  *out = static_cast<float>((adc_voltage * 3.0 / 1000.0) / BATTERY_ADC_OFFSET);
}
