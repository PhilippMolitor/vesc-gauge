#pragma once

#include <stdint.h>
#include <stddef.h>

constexpr uint8_t ANALOG_READ_RESOLUTION = 12;

// external IO expander address
constexpr size_t PIN_PCF8575_INTERRUPT = 0;
constexpr uint8_t PCF8575_ADDRESS = 0x20;

// external IO expander pins
constexpr uint8_t PCF8575_PIN_LIGHT_HEADLIGHT = 1; // TODO
constexpr uint8_t PCF8575_PIN_LIGHT_TAILLIGHT = 2; // TODO
constexpr uint8_t PCF8575_PIN_LIGHT_BRAKELIGHT = 3; // TODO
constexpr uint8_t PCF8575_PIN_LIGHT_TURNSIGNAL_LEFT = 4; // TODO
constexpr uint8_t PCF8575_PIN_LIGHT_TURNSIGNAL_RIGHT = 5; // TODO

constexpr uint8_t PCF8575_PIN_BUTTON_PROFILE_UP = 6; // TODO
constexpr uint8_t PCF8575_PIN_BUTTON_PROFILE_DOWN = 7; // TODO
constexpr uint8_t PCF8575_PIN_SWITCH_LIGHTS_SWITCH = 8; // TODO
constexpr uint8_t PCF8575_PIN_SWITCH_TURNSIGNAL_LEFT = 8; // TODO
constexpr uint8_t PCF8575_PIN_SWITCH_TURNSIGNAL_RIGHT = 9; // TODO