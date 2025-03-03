#include "wled_esp_now.h"

static const char* LOG_TAG = "wled_esp_now";

static bool state_initialized = false;
static esp_now_peer_info_t state_espnow_peer_info;
static bool state_tx_started = false;
static bool state_tx_ack = false;

static void wled_tx_cb(const uint8_t* mac_addr, esp_now_send_status_t status)
{
  ESP_LOGV(LOG_TAG, "tx status: %s", status == ESP_NOW_SEND_SUCCESS ? "success" : "fail");
  state_tx_ack = true;
}

static void wled_esp_now_send_packet(const wled_esp_now_packet* p)
{
  // repeatedly send packet for reliability
  for (uint8_t r = 0; r < WLED_ESP_NOW_TX_REPEATS; r++) {
    state_tx_ack = false;

    auto send_error = esp_now_send(
        state_espnow_peer_info.peer_addr,
        reinterpret_cast<const uint8_t*>(p),
        sizeof(*p));

    if (send_error != ESP_OK) {
      ESP_LOGE(LOG_TAG, "failed to send message (repeat %d): error %d", r, send_error);
      continue;
    }

    ESP_LOGV(LOG_TAG, "packet queued (repeat %d)", r);

    auto timeout = esp_timer_get_time() + WLED_ESP_NOW_TX_TIMEOUT;
    while (esp_timer_get_time() < timeout && state_tx_ack == false)
      ;

    if (state_tx_ack == false) {
      ESP_LOGE(LOG_TAG, "timeout waiting for ack (repeat %d)", r);
    }

// makes sending more reliable if the interface is busy
#if WLED_ESP_NOW_TX_REPEATS > 1
    delayMicroseconds(50);
#endif
  }
}

uint8_t wled_esp_now_init()
{
  if (state_initialized) {
    ESP_LOGE(LOG_TAG, "already initialized");
    return 1;
  }

  // without this, wifi won't work properly
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  // initialize wifi
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  cfg.nvs_enable = 0;
  ESP_RETURN_ON_ERROR(esp_wifi_init(&cfg), LOG_TAG, "failed to configure wifi");
  ESP_RETURN_ON_ERROR(esp_wifi_set_storage(WIFI_STORAGE_RAM), LOG_TAG, "failed to set wifi storage");
  ESP_RETURN_ON_ERROR(esp_wifi_set_mode(wifi_mode_t::WIFI_MODE_STA), LOG_TAG, "failed to set wifi mode");
  ESP_RETURN_ON_ERROR(esp_wifi_start(), LOG_TAG, "failed to start wifi");
  ESP_RETURN_ON_ERROR(esp_wifi_set_max_tx_power(15 * 4), LOG_TAG, "failed to set tx power"); // 1 = 0.25dBm

  // start esp_now and register esp_now callbacks
  ESP_RETURN_ON_ERROR(esp_now_init(), LOG_TAG, "failed to initialize esp_now");
  ESP_RETURN_ON_ERROR(esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE), LOG_TAG, "failed to set wifi channel to %d", state_espnow_peer_info.channel);
  esp_now_register_send_cb(wled_tx_cb);

  // add peer
  memcpy(state_espnow_peer_info.peer_addr, wled_broadcast_addr, 6);
  state_espnow_peer_info.channel = 0;
  state_espnow_peer_info.encrypt = false;
  ESP_RETURN_ON_ERROR(esp_now_add_peer(&state_espnow_peer_info), LOG_TAG, "failed to add peer");

  // log mac address and state
  uint8_t mac_addr[6];
  wled_esp_now_mac_get(mac_addr);
  ESP_LOGI(LOG_TAG, "initialized, mac: %02x%02x%02x%02x%02x%02x",
      mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  state_initialized = true;
  return 0;
}

uint8_t wled_esp_now_channel_set(uint8_t channel)
{
  if (!state_initialized) {
    ESP_LOGE(LOG_TAG, "not initialized");
    return 1;
  }

  if (channel < WLED_ESP_NOW_CHANNEL_MIN - 1 || channel > WLED_ESP_NOW_CHANNEL_MAX) {
    ESP_LOGE(LOG_TAG, "invalid channel: %d", channel);
    return 2;
  }

  state_espnow_peer_info.channel = channel;
  if (channel > 0)
    ESP_RETURN_ON_ERROR(
        esp_wifi_set_channel(state_espnow_peer_info.channel, WIFI_SECOND_CHAN_NONE),
        LOG_TAG,
        "failed to set wifi channel to %d",
        state_espnow_peer_info.channel);
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

  static uint32_t seq = 0;
  seq++;

  wled_esp_now_packet p;
  p.seq[0] = byte(seq);
  p.seq[1] = byte(seq >> 8);
  p.seq[2] = byte(seq >> 16);
  p.seq[3] = byte(seq >> 24);
  p.button = cmd;
  p.program = cmd == wled_esp_now_cmd::POWER_ON ? 0x91 : 0x81;

  ESP_LOGI(LOG_TAG, "sending packet: %d (seq %d)", p.button, seq);

  // if a channel is set, only send to that channel
  if (state_espnow_peer_info.channel != 0) {
    wled_esp_now_send_packet(&p);
    return 0;
  }

  // for each wifi channel
  for (uint8_t c = WLED_ESP_NOW_CHANNEL_MIN; c <= WLED_ESP_NOW_CHANNEL_MAX; c++) {
    ESP_RETURN_ON_ERROR(
        esp_wifi_set_channel(c, WIFI_SECOND_CHAN_NONE),
        LOG_TAG,
        "failed to set wifi channel to %d",
        c);

    wled_esp_now_send_packet(&p);
  }

  return 0;
}
