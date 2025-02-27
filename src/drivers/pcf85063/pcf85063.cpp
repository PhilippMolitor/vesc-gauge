#include "pcf85063.h"

#include <esp_log.h>

static const char* LOG_TAG = "rtc";

static uint8_t dec_to_bcd(uint8_t dec)
{
  return ((dec / 10) << 4) | (dec % 10);
}

static uint8_t bcd_to_dec(uint8_t bcd)
{
  return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

uint8_t pcf85063_init()
{
  uint8_t buf = RTC_CTRL_1_DEFAULT | RTC_CTRL_1_CAP_SEL;
  i2c_write(PCF85063_ADDRESS, RTC_CTRL_1_ADDR, &buf, 1);
  i2c_read(PCF85063_ADDRESS, RTC_CTRL_1_ADDR, &buf, 1);

  if (buf & RTC_CTRL_1_STOP) {
    ESP_LOGE(LOG_TAG, "init failed");
    return 1;
  }

  ESP_LOGI(LOG_TAG, "initialized", buf);
  return 0;
}

uint8_t pcf85063_reset()
{
  uint8_t value = RTC_CTRL_1_DEFAULT | RTC_CTRL_1_CAP_SEL | RTC_CTRL_1_SR;
  esp_err_t ret = i2c_write(PCF85063_ADDRESS, RTC_CTRL_1_ADDR, &value, 1);

  if (ret != ESP_OK) {
    ESP_LOGE(LOG_TAG, "reset failed");
    return 1;
  }

  return 0;
}

uint8_t pcf85063_set_date(datetime_t date)
{
  uint8_t buf[4] = { dec_to_bcd(date.day),
    dec_to_bcd(date.dotw),
    dec_to_bcd(date.month),
    dec_to_bcd(date.year - YEAR_OFFSET) };
  esp_err_t ret = i2c_write(PCF85063_ADDRESS, RTC_DAY_ADDR, buf, sizeof(buf));

  if (ret != ESP_OK) {
    ESP_LOGE(LOG_TAG, "failed to set date");
    return 1;
  }

  return 0;
}

uint8_t pcf85063_set_time(datetime_t time)
{
  uint8_t buf[3] = { dec_to_bcd(time.second),
    dec_to_bcd(time.minute),
    dec_to_bcd(time.hour) };
  esp_err_t ret = i2c_write(PCF85063_ADDRESS, RTC_SECOND_ADDR, buf, sizeof(buf));

  if (ret != ESP_OK) {
    ESP_LOGE(LOG_TAG, "failed to set time");
    return 1;
  }

  return 0;
}

uint8_t pcf85063_set(datetime_t datetime)
{
  uint8_t buf[7] = { dec_to_bcd(datetime.second),
    dec_to_bcd(datetime.minute),
    dec_to_bcd(datetime.hour),
    dec_to_bcd(datetime.day),
    dec_to_bcd(datetime.dotw),
    dec_to_bcd(datetime.month),
    dec_to_bcd(datetime.year - YEAR_OFFSET) };
  esp_err_t ret = i2c_write(PCF85063_ADDRESS, RTC_SECOND_ADDR, buf, sizeof(buf));

  if (ret != ESP_OK) {
    ESP_LOGE(LOG_TAG, "failed to set datetime");
    return 1;
  }

  return 0;
}

uint8_t pcf85063_read(datetime_t* datetime)
{
  uint8_t buf[7] = { 0 };
  esp_err_t ret = i2c_read(PCF85063_ADDRESS, RTC_SECOND_ADDR, buf, sizeof(buf));

  if (ret != ESP_OK) {
    ESP_LOGE(LOG_TAG, "failed to read datetime");
    return 1;
  }

  datetime->second = bcd_to_dec(buf[0]);
  datetime->minute = bcd_to_dec(buf[1]);
  datetime->hour = bcd_to_dec(buf[2]);
  datetime->day = bcd_to_dec(buf[3]);
  datetime->dotw = bcd_to_dec(buf[4]);
  datetime->month = bcd_to_dec(buf[5]);
  datetime->year = bcd_to_dec(buf[6]) + YEAR_OFFSET;

  return 0;
}

uint8_t pcf85063_alarm_enable()
{
  uint8_t value = RTC_CTRL_2_DEFAULT | RTC_CTRL_2_AIE;
  value &= ~RTC_CTRL_2_AF;
  esp_err_t ret = i2c_write(PCF85063_ADDRESS, RTC_CTRL_2_ADDR, &value, 1);

  if (ret != ESP_OK) {
    ESP_LOGE(LOG_TAG, "failed to enable alarm");
    return 1;
  }

  return 0;
}

uint8_t pcf85063_alarm_status(uint8_t* status)
{
  uint8_t value = 0;
  esp_err_t ret = i2c_read(PCF85063_ADDRESS, RTC_CTRL_2_ADDR, &value, 1);

  if (ret != ESP_OK) {
    ESP_LOGE(LOG_TAG, "failed to read alarm status");
    return 1;
  }

  *status = value & (RTC_CTRL_2_AF | RTC_CTRL_2_AIE);
  return 0;
}

uint8_t pcf85063_alarm_set(datetime_t time)
{
  uint8_t buf[5] = {
    dec_to_bcd(time.second) & (~RTC_ALARM),
    dec_to_bcd(time.minute) & (~RTC_ALARM),
    dec_to_bcd(time.hour) & (~RTC_ALARM),
    RTC_ALARM,
    RTC_ALARM
  };
  esp_err_t ret = i2c_write(PCF85063_ADDRESS, RTC_SECOND_ALARM, buf, sizeof(buf));

  if (ret != ESP_OK) {
    ESP_LOGE(LOG_TAG, "failed to set alarm time");
    return 1;
  }

  return 0;
}

uint8_t pcf85063_alarm_read(datetime_t* time)
{
  uint8_t buf[5] = { 0 };
  esp_err_t ret = i2c_read(PCF85063_ADDRESS, RTC_SECOND_ALARM, buf, sizeof(buf));

  if (ret != ESP_OK) {
    ESP_LOGE(LOG_TAG, "failed to read alarm time");
    return 1;
  }

  time->second = bcd_to_dec(buf[0] & (~RTC_ALARM));
  time->minute = bcd_to_dec(buf[1] & (~RTC_ALARM));
  time->hour = bcd_to_dec(buf[2] & (~RTC_ALARM));
  time->day = bcd_to_dec(buf[3] & (~RTC_ALARM));
  time->dotw = bcd_to_dec(buf[4] & (~RTC_ALARM));

  return 0;
}
