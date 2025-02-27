#include "tca9554pwr.h"

static const char* LOG_TAG = "io_expander";

static uint8_t tca9554pwr_i2c_read(uint8_t addr) // Read the value of the TCA9554PWR register REG
{
  Wire.beginTransmission(TCA9554PWR_ADDRESS);
  Wire.write(addr);
  uint8_t result = Wire.endTransmission();

  if (result != 0)
    ESP_LOGE(LOG_TAG, "I2C transmission failed");

  Wire.requestFrom(TCA9554PWR_ADDRESS, 1);

  uint8_t status;
  if (Wire.available())
    status = Wire.read();

  return status;
}

static uint8_t tca9554pwr_i2c_write(uint8_t addr, uint8_t data) // Write Data to the REG register of the TCA9554PWR
{
  Wire.beginTransmission(TCA9554PWR_ADDRESS);
  Wire.write(addr);
  Wire.write(data);
  uint8_t result = Wire.endTransmission();

  if (result != 0) {
    ESP_LOGE(LOG_TAG, "I2C transmission failed");
    return -1;
  }

  return 0;
}

void tca9554pwr_init(uint8_t state)
{
  tca9554pwr_configure_all(state);
}

void tca9554pwr_configure(uint8_t pin, uint8_t state)
{
  uint8_t status = tca9554pwr_i2c_read(TCA9554PWR_REG_CONFIG);
  uint8_t data = (0x01 << (pin - 1)) | status;
  uint8_t result = tca9554pwr_i2c_write(TCA9554PWR_REG_CONFIG, data);
  if (result != 0) {
    ESP_LOGE(LOG_TAG, "I/O configuration failure");
  }
}

void tca9554pwr_configure_all(uint8_t state)
{
  uint8_t result = tca9554pwr_i2c_write(TCA9554PWR_REG_CONFIG, state);
  if (result != 0)
    ESP_LOGE(LOG_TAG, "I/O configuration failure");
}

uint8_t tca9554pwr_read(uint8_t pin)
{
  uint8_t status = tca9554pwr_i2c_read(TCA9554PWR_REG_INPUT);
  return (status >> (pin - 1)) & 0x01;
}

uint8_t tca9554pwr_read_all(uint8_t reg = TCA9554PWR_REG_INPUT)
{
  return tca9554pwr_i2c_read(reg);
}

void tca9554pwr_write(uint8_t pin, uint8_t state)
{
  if (state > 1 || pin > 8 || pin < 1) {
    ESP_LOGE(LOG_TAG, "invalid pin: %d", pin);
    return;
  }

  uint8_t status = tca9554pwr_read_all(TCA9554PWR_REG_OUTPUT);
  uint8_t data;

  if (state == 1)
    data = (0x01 << (pin - 1)) | status;
  else if (state == 0)
    data = (~(0x01 << (pin - 1))) & status;

  uint8_t result = tca9554pwr_i2c_write(TCA9554PWR_REG_OUTPUT, data);

  if (result != 0)
    ESP_LOGE(LOG_TAG, "failed to set GPIO state");
}

void tca9554pwr_write_all(uint8_t state)
{
  uint8_t result = tca9554pwr_i2c_write(TCA9554PWR_REG_OUTPUT, state);
  if (result != 0)
    ESP_LOGE(LOG_TAG, "failed to set GPIO state");
}

void tca9554pwr_toggle(uint8_t pin)
{
  uint8_t status = tca9554pwr_read(pin);
  tca9554pwr_write(pin, (bool)!status);
}
