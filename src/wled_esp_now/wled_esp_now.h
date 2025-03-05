#pragma once

#include <Arduino.h>

#define WLED_ESP_NOW_CHANNEL_MIN (1u)
#define WLED_ESP_NOW_CHANNEL_MAX (14u)

#define WLED_ESP_NOW_COUNT_PRESETS_DEFAULT (4u)
#define WLED_ESP_NOW_COUNT_PRESETS (10u)
#define WLED_ESP_NOW_COUNT_BRIGHTNESS (10u)

enum wled_wizmote_cmd : uint8_t {
  POWER_ON = 1,
  POWER_OFF = 2,
  SMART_ON = 100,
  SMART_OFF = 101,

  BRIGHTNESS_DOWN = 8,
  BRIGHTNESS_UP = 9,
  SMART_BRIGHT_UP = 102,
  SMART_BRIGHT_DOWN = 103,

  NIGHT = 3,

  PRESET_1 = 16, // Offsettable presets (PRESET_1 + offset)
  PRESET_2, // 17...
  PRESET_3,
  PRESET_4, // ...19

  //
  // extended commands - only available with custom remote.json uploaded to WLED
  //

  POWER_TOGGLE = 4,

  PRESET_5 = 20, // Extended offsettable presets (PRESET_1 + offset)
  PRESET_6, // 21...
  PRESET_7,
  PRESET_8,
  PRESET_9,
  PRESET_10,
  PRESET_11,
  PRESET_12,
  PRESET_13,
  PRESET_14,
  PRESET_15,
  PRESET_16, // ...31

  BRIGHTNESS_1 = 64, // Offsettable brightness (BRIGHTNESS_1 + offset)
  BRIGHTNESS_2, // 65...
  BRIGHTNESS_3,
  BRIGHTNESS_4,
  BRIGHTNESS_5,
  BRIGHTNESS_6,
  BRIGHTNESS_7,
  BRIGHTNESS_8,
  BRIGHTNESS_9,
  BRIGHTNESS_10, // ...73
};

uint8_t wled_esp_now_init();
uint8_t wled_esp_now_channel_set(uint8_t channel);
uint8_t wled_esp_now_mac_get(uint8_t* mac_addr);

uint8_t wled_esp_now_wizmote_cmd(wled_wizmote_cmd cmd);
