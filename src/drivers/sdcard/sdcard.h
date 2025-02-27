#pragma once

#include <cstring>

#include <Arduino.h>

#include <FS.h>
#include <SD_MMC.h>

#include "../tca9554pwr/tca9554pwr.h"

uint8_t sdcard_mount(size_t* size = nullptr, size_t* used = nullptr, const char* mountpoint = "/sdcard");
void sdcard_unmount();
bool sdcard_file_exists(const char* directory, const char* name);
