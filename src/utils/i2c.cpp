#include "i2c.h"

bool i2c_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len)
{
  Wire.beginTransmission(addr);

  Wire.write(reg);
  if (Wire.endTransmission(true))
    return -1;

  Wire.requestFrom(addr, len);
  for (int i = 0; i < len; i++)
    *data++ = Wire.read();

  return 0;
}

bool i2c_write(uint8_t addr, uint8_t reg, const uint8_t* data, uint32_t len)
{
  Wire.beginTransmission(addr);

  Wire.write(reg);
  for (int i = 0; i < len; i++)
    Wire.write(*data++);

  return Wire.endTransmission(true) ? -1 : 0;
}
