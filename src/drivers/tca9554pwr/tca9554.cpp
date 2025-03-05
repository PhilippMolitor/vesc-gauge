#include "tca9554.h"

#include <esp_log.h>

#include <Wire.h>

#include "../../utils/i2c.h"

static const char* LOG_TAG = "io_expander";

void tca9554pwr_init(uint8_t state)
{
  tca9554pwr_configure_all(state);
}

void tca9554pwr_configure(uint8_t pin, const uint8_t state)
{
  uint8_t status;
  i2c_read(TCA9554PWR_ADDRESS, TCA9554PWR_REG_CONFIG, &status, 1);
  uint8_t data = (0x01 << (pin - 1)) | status;
  auto result = i2c_write(TCA9554PWR_ADDRESS, TCA9554PWR_REG_CONFIG, &data, 1);
  if (result != 0) {
    ESP_LOGE(LOG_TAG, "I/O configuration failure");
  }
}

void tca9554pwr_configure_all(uint8_t state)
{
  uint8_t result = i2c_write(TCA9554PWR_ADDRESS, TCA9554PWR_REG_CONFIG, &state, 1);
  if (result != 0)
    ESP_LOGE(LOG_TAG, "I/O configuration failure");
}

uint8_t tca9554pwr_read(uint8_t pin)
{
  uint8_t status;
  i2c_read(TCA9554PWR_ADDRESS, TCA9554PWR_REG_INPUT, &status, 1);
  return (status >> (pin - 1)) & 0x01;
}

uint8_t tca9554pwr_read_all(uint8_t reg = TCA9554PWR_REG_INPUT)
{
  uint8_t result;
  i2c_read(TCA9554PWR_ADDRESS, reg, &result, 1);
  return result;
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

  uint8_t result = i2c_write(TCA9554PWR_ADDRESS, TCA9554PWR_REG_OUTPUT, &data, 1);

  if (result != 0)
    ESP_LOGE(LOG_TAG, "failed to set GPIO state");
}

void tca9554pwr_write_all(uint8_t state)
{
  uint8_t result = i2c_write(TCA9554PWR_ADDRESS, TCA9554PWR_REG_OUTPUT, &state, 1);
  if (result != 0)
    ESP_LOGE(LOG_TAG, "failed to set GPIO state");
}

void tca9554pwr_toggle(uint8_t pin)
{
  uint8_t status = tca9554pwr_read(pin);
  tca9554pwr_write(pin, (bool)!status);
}
