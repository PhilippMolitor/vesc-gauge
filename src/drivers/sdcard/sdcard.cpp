#include "sdcard.h"

#include <esp_log.h>

#include <Arduino.h>

#include <FS.h>
#include <SD_MMC.h>

#include "../tca9554pwr/tca9554.h"

static const char* LOG_TAG = "sdcard";

static void sdcard_cs_set(uint8_t state)
{
  tca9554pwr_write(PIN_TCA9554PWR_SD_SPI_CS, state);
  vTaskDelay(pdMS_TO_TICKS(10));
}

uint8_t sdcard_mount(size_t* size, size_t* used, const char* mountpoint)
{
  if (!SD_MMC.setPins(SCK, MOSI, PIN_SD_SPI_D0, -1, -1, -1)) {
    ESP_LOGE(LOG_TAG, "failed to set pins");
    return 1;
  }

  sdcard_cs_set(HIGH);

  if (SD_MMC.begin(mountpoint, true, true)) {
    ESP_LOGI(LOG_TAG, "mounted on %s", mountpoint);
  } else {
    ESP_LOGE(LOG_TAG, "mounting failed");
    return 2;
  }

  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    ESP_LOGW(LOG_TAG, "no SD card found");
    return 3;
  }

  if (size != nullptr)
    *size = SD_MMC.totalBytes();
  if (used != nullptr)
    *used = SD_MMC.usedBytes();

  return 0;
}

void sdcard_unmount()
{
  SD_MMC.end();
  ESP_LOGI(LOG_TAG, "unmounted");
}

bool sdcard_file_exists(const char* directory, const char* name)
{
  File dir = SD_MMC.open(directory);
  if (!dir)
    return false;

  File file = dir.openNextFile();
  while (file) {
    if (strcmp(file.name(), name) == 0) {
      dir.close();
      return true;
    }

    file = dir.openNextFile();
  }

  dir.close();

  return false;
}
