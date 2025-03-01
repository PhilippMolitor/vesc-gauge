#include "wled_esp_now.h"

static const char* LOG_TAG = "wled_esp_now";

static bool state_initialized = false;
static esp_now_peer_info_t state_espnow_peer_info;
static uint32_t state_packet_sequence = 0;
static uint8_t state_packet_retries = 0;
static bool state_tx_started = false;
static bool state_tx_ack = false;

static void wled_tx_cb(const uint8_t* mac_addr, esp_now_send_status_t status)
{
  state_tx_ack = true;
}

uint8_t wled_esp_now_init(uint8_t channel)
{
  if (state_initialized) {
    ESP_LOGE(LOG_TAG, "already initialized");
    return 1;
  }

  // initialize wifi
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  cfg.nvs_enable = 0;
  ESP_RETURN_ON_ERROR(esp_wifi_init(&cfg), LOG_TAG, "failed to configure wifi");
  ESP_RETURN_ON_ERROR(esp_wifi_set_storage(WIFI_STORAGE_RAM), LOG_TAG, "failed to set wifi storage");
  ESP_RETURN_ON_ERROR(esp_wifi_set_mode(wifi_mode_t::WIFI_MODE_STA), LOG_TAG, "failed to set wifi mode");
  ESP_RETURN_ON_ERROR(esp_wifi_start(), LOG_TAG, "failed to start wifi");
  ESP_RETURN_ON_ERROR(esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE), LOG_TAG, "failed to set wifi channel");
  ESP_RETURN_ON_ERROR(esp_wifi_set_max_tx_power(15 * 4), LOG_TAG, "failed to set tx power"); // 1 = 0.25dBm

  if (esp_now_init() != ESP_OK) {
    ESP_LOGE(LOG_TAG, "failed to initialize esp_now");
    return 2;
  }

  // register esp_now callbacks
  esp_now_register_send_cb(wled_tx_cb);

  // set up peer info
  memcpy(state_espnow_peer_info.peer_addr, wled_broadcast_addr, 6);
  state_espnow_peer_info.channel = channel;
  state_espnow_peer_info.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&state_espnow_peer_info) != ESP_OK) {
    ESP_LOGE(LOG_TAG, "failed to add peer");
    return 3;
  }

  uint8_t mac_addr[6];
  wled_esp_now_mac_get(mac_addr);
  ESP_LOGI(LOG_TAG, "initialized, mac: %02x%02x%02x%02x%02x%02x channel: %d",
      mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5], channel);

  state_initialized = true;
  return 0;
}

uint8_t wled_esp_now_deinit()
{
  esp_now_deinit();
  esp_wifi_stop();
  esp_wifi_deinit();

  state_initialized = false;
  return 0;
}

uint8_t wled_esp_now_mac_get(uint8_t* mac_addr)
{
  esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
  return 0;
}

uint8_t wled_esp_now_send(wled_esp_now_cmd cmd)
{

  if (!state_initialized) {
    ESP_LOGE(LOG_TAG, "not initialized");
    return 1;
  }

  state_tx_ack = false;
  state_packet_sequence += 1;

  wled_esp_now_packet p;
  p.seq[0] = state_packet_sequence;
  p.seq[1] = state_packet_sequence >> 8;
  p.seq[2] = state_packet_sequence >> 16;
  p.seq[3] = state_packet_sequence >> 24;

  p.button = cmd;
  if (cmd == wled_esp_now_cmd::POWER_ON)
    p.program = 0x91;

  auto send_error = esp_now_send(
      state_espnow_peer_info.peer_addr,
      (uint8_t*)&p,
      sizeof(p));

  if (send_error != ESP_OK) {
    ESP_LOGE(LOG_TAG, "failed to send message");
    return 2;
  }

  auto timeout = esp_timer_get_time() + WLED_ESP_NOW_TX_TIMEOUT;
  while (esp_timer_get_time() < timeout && state_tx_ack == false)
    ;

  return 0;
}
