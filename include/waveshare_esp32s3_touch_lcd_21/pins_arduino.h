#pragma once

#include "soc/soc_caps.h"
#include <stdint.h>

// BN: ESP32 Family Device
#define USB_VID 0x303a
#define USB_PID 0x825E

#ifndef USB_MANUFACTURER
#define USB_MANUFACTURER "Waveshare"
#endif
#ifndef USB_PRODUCT
#define USB_PRODUCT "ESP32-S3-Touch-LCD-21"
#endif
#ifndef USB_SERIAL
#define USB_SERIAL ""
#endif

// SPI - these are used by internal libraries of arduino-espressif32
static const uint8_t SS = -1; // FSPICS0
static const uint8_t MISO = -1; // FSPIQ
static const uint8_t MOSI = 1; // FSPID
static const uint8_t SCK = 2; // FSPICLK

// I2C - these are used by internal libraries of arduino-espressif32
static const uint8_t SDA = 15;
static const uint8_t SCL = 7;

// Serial - either via USB or direct pins
static const uint8_t PIN_SERIAL_TX = 43;
static const uint8_t PIN_SERIAL_RX = 44;

// SD card
static const uint8_t PIN_SD_SPI_D0 = 42;

// Li-Ion battery voltage
static const uint8_t PIN_BATTERY_ADC = 4;

// LCD
static const uint8_t PIN_LCD_BACKLIGHT = 6;
static const uint8_t PIN_LCD_RGB_HSYNC = 38;
static const uint8_t PIN_LCD_VSYNC = 39;
static const uint8_t PIN_LCD_DE = 40;
static const uint8_t PIN_LCD_PCLK = 41;
static const uint8_t PIN_LCD_DATA0 = 5;
static const uint8_t PIN_LCD_DATA1 = 45;
static const uint8_t PIN_LCD_DATA2 = 48;
static const uint8_t PIN_LCD_DATA3 = 47;
static const uint8_t PIN_LCD_DATA4 = 21;
static const uint8_t PIN_LCD_DATA5 = 14;
static const uint8_t PIN_LCD_DATA6 = 13;
static const uint8_t PIN_LCD_DATA7 = 12;
static const uint8_t PIN_LCD_DATA8 = 11;
static const uint8_t PIN_LCD_DATA9 = 10;
static const uint8_t PIN_LCD_DATA10 = 9;
static const uint8_t PIN_LCD_DATA11 = 46;
static const uint8_t PIN_LCD_DATA12 = 3;
static const uint8_t PIN_LCD_DATA13 = 8;
static const uint8_t PIN_LCD_DATA14 = 18;
static const uint8_t PIN_LCD_DATA15 = 17;

// Touch
static const uint8_t PIN_TOUCH_INTERRUPT = 16;

// IO expander
static const uint8_t PIN_TCA9554PWR_LCD_RESET = 1;
static const uint8_t PIN_TCA9554PWR_TOUCH_RESET = 2;
static const uint8_t PIN_TCA9554PWR_LCD_SPI_CS = 3;
static const uint8_t PIN_TCA9554PWR_SD_SPI_CS = 4;
// Pins 5, 6 and 7 are unused
static const uint8_t PIN_TCA9554PWR_BUZZER = 8;
