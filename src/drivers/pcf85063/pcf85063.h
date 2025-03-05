#pragma once

#include <inttypes.h>

/*
weekday format:
0 - sunday
1 - monday
2 - tuesday
3 - wednesday
4 - thursday
5 - friday
6 - saturday
*/
typedef struct {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t dotw;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} datetime_t;

uint8_t pcf85063_init(void);
uint8_t pcf85063_reset(void);

uint8_t pcf85063_set_date(datetime_t date);
uint8_t pcf85063_set_time(datetime_t time);
uint8_t pcf85063_set(datetime_t datetime);
uint8_t pcf85063_read(datetime_t* time);

uint8_t pcf85063_alarm_enable(void);
uint8_t pcf85063_alarm_status(uint8_t* status);
uint8_t pcf85063_alarm_set(datetime_t time);
uint8_t pcf85063_alarm_read(datetime_t* time);
