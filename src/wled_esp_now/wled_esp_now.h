#pragma once

#include <esp_check.h>
#include <esp_log.h>
#include <esp_mac.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include <Arduino.h>

#define WLED_ESP_NOW_CHANNEL_MIN (1u)
#define WLED_ESP_NOW_CHANNEL_MAX (14u)
#define WLED_ESP_NOW_TX_REPEATS (3u)
#define WLED_ESP_NOW_TX_TIMEOUT (10000u) // 10000uS = 10ms

static constexpr uint8_t wled_broadcast_addr[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

typedef struct wled_esp_now_packet {
  uint8_t program; // 0x91 for ON button, 0x81 for all others
  uint8_t seq[4]; // Incremetal sequence number 32 bit unsigned integer LSB first
  uint8_t dt1 = 0x32; // Button Data Type (0x32)
  uint8_t button; // Identifies which button is being pressed
  uint8_t dt2 = 0x01; // Battery Level Data Type (0x01)
  uint8_t batLevel = 90; // Battery Level 0-100
  uint8_t byte10; // Unknown, maybe checksum
  uint8_t byte11; // Unknown, maybe checksum
  uint8_t byte12; // Unknown, maybe checksum
  uint8_t byte13; // Unknown, maybe checksum
} wled_esp_now_packet;

enum wled_esp_now_cmd : uint8_t {
  POWER_ON = 1,
  POWER_OFF = 2,
  NIGHT = 3,
  BRIGHTNESS_DOWN = 8,
  BRIGHTNESS_UP = 9,
  PRESET_1 = 16,
  PRESET_2 = 17,
  PRESET_3 = 18,
  PRESET_4 = 19,
};

uint8_t wled_esp_now_init();
uint8_t wled_esp_now_channel_set(uint8_t channel);
uint8_t wled_esp_now_mac_get(uint8_t* mac_addr);
uint8_t wled_esp_now_send(wled_esp_now_cmd cmd);
