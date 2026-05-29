#include "vehicle.h"

#include <Arduino.h>
#include <FunctionalInterrupt.h>

Vehicle::Lights::Lights(Vehicle& vehicle)
    : m_vehicle(vehicle)
    , m_stateHeadlight(false)
    , m_stateTaillight(false)
    , m_stateBrakeLight(false)
    , m_stateTurnSignal(Vehicle::Lights::TurnSignal::OFF)
{
}

bool Vehicle::Lights::getHeadlight()
{
  return m_stateHeadlight;
}

void Vehicle::Lights::setHeadlight(bool state)
{
  m_stateHeadlight = state;
  m_vehicle.m_pcf8575->write(PCF8575_PIN_LIGHT_HEADLIGHT, state);
}

bool Vehicle::Lights::getTailLight()
{
  return m_stateTaillight;
}

void Vehicle::Lights::setTailLight(bool state)
{
  m_stateTaillight = state;
  m_vehicle.m_pcf8575->write(PCF8575_PIN_LIGHT_TAILLIGHT, state);
}

bool Vehicle::Lights::getBrakeLight()
{
  return m_stateBrakeLight;
}

void Vehicle::Lights::setBrakeLight(bool state)
{
  m_stateBrakeLight = state;
  m_vehicle.m_pcf8575->write(PCF8575_PIN_LIGHT_BRAKELIGHT, state);
}

Vehicle::Lights::TurnSignal Vehicle::Lights::getTurnSignal()
{
  return m_stateTurnSignal;
}

void Vehicle::Lights::setTurnSignal(Vehicle::Lights::TurnSignal signal)
{
  m_stateTurnSignal = signal;
  m_vehicle.m_pcf8575->write(PCF8575_PIN_LIGHT_TURNSIGNAL_LEFT, signal == Vehicle::Lights::TurnSignal::LEFT ? 1 : 0);
  m_vehicle.m_pcf8575->write(PCF8575_PIN_LIGHT_TURNSIGNAL_RIGHT, signal == Vehicle::Lights::TurnSignal::RIGHT ? 1 : 0);
}

Vehicle::Controls::Controls(Vehicle& vehicle)
    : m_vehicle(vehicle)
    , m_needsRead(false)
    , m_updates(static_cast<uint8_t>(Update::NONE))
    , m_profileChangeCallback(nullptr)
    , m_lightsCallback(nullptr)
    , m_turnSignalCallback(nullptr)
{
}

void Vehicle::Controls::begin()
{
  attachInterrupt(digitalPinToInterrupt(PIN_PCF8575_INTERRUPT), [this]() { m_needsRead = true; }, FALLING);
}

void Vehicle::Controls::read()
{
  m_needsRead = false;
  auto data = m_vehicle.m_pcf8575->read16();

  if (data & (1 << PCF8575_PIN_BUTTON_PROFILE_DOWN))
    m_updates |= static_cast<uint8_t>(Update::PROFILE_CHANGE_DOWN);
  else if (data & (1 << PCF8575_PIN_BUTTON_PROFILE_UP))
    m_updates |= static_cast<uint8_t>(Update::PROFILE_CHANGE_UP);

  // data is set at bit, so it is on
  if (data & (1 << PCF8575_PIN_SWITCH_LIGHTS_SWITCH))
    m_updates |= static_cast<uint8_t>(Update::LIGHTS_ON);
  // data at bit is not set, so it is off
  else if (!(data & (1 << PCF8575_PIN_SWITCH_LIGHTS_SWITCH)))
    m_updates |= static_cast<uint8_t>(Update::LIGHTS_OFF);

  if (data & (1 << PCF8575_PIN_SWITCH_TURNSIGNAL_LEFT))
    m_updates |= static_cast<uint8_t>(Update::TURNSIGNAL_LEFT);
  if (data & (1 << PCF8575_PIN_SWITCH_TURNSIGNAL_RIGHT))
    m_updates |= static_cast<uint8_t>(Update::TURNSIGNAL_RIGHT);
}

void Vehicle::Controls::update()
{
  if (m_needsRead) {
    read();
    m_needsRead = false;
  }

  if (m_profileChangeCallback && (m_updates & static_cast<uint8_t>(Update::PROFILE_CHANGE_DOWN))) {
    m_profileChangeCallback(false);
    m_updates &= ~static_cast<uint8_t>(Update::PROFILE_CHANGE_DOWN);
  }

  if (m_profileChangeCallback && (m_updates & static_cast<uint8_t>(Update::PROFILE_CHANGE_UP))) {
    m_profileChangeCallback(true);
    m_updates &= ~static_cast<uint8_t>(Update::PROFILE_CHANGE_UP);
  }

  if (m_lightsCallback && (m_updates & static_cast<uint8_t>(Update::LIGHTS_OFF))) {
    m_lightsCallback(true);
    m_updates &= ~static_cast<uint8_t>(Update::LIGHTS_OFF);
  }

  if (m_lightsCallback && (m_updates & static_cast<uint8_t>(Update::LIGHTS_ON))) {
    m_lightsCallback(false);
    m_updates &= ~static_cast<uint8_t>(Update::LIGHTS_ON);
  }

  if (m_turnSignalCallback && (m_updates & static_cast<uint8_t>(Update::TURNSIGNAL_LEFT))) {
    m_turnSignalCallback(Vehicle::Lights::TurnSignal::LEFT);
    m_updates &= ~static_cast<uint8_t>(Update::TURNSIGNAL_LEFT);
  }

  if (m_turnSignalCallback && (m_updates & static_cast<uint8_t>(Update::TURNSIGNAL_RIGHT))) {
    m_turnSignalCallback(Vehicle::Lights::TurnSignal::RIGHT);
    m_updates &= ~static_cast<uint8_t>(Update::TURNSIGNAL_RIGHT);
  }
}

void Vehicle::Controls::onProfileChange(Vehicle::Controls::ProfileChangeCallback callback)
{
  m_profileChangeCallback = callback;
}

void Vehicle::Controls::onLightsSet(Vehicle::Controls::LightsCallback callback)
{
  m_lightsCallback = callback;
}

void Vehicle::Controls::onTurnSignalSet(Vehicle::Controls::TurnSignalCallback callback)
{
  m_turnSignalCallback = callback;
}

Vehicle::Vehicle() { }

void Vehicle::begin()
{
  m_pcf8575 = new PCF8575(PCF8575_ADDRESS, &Wire);
  controls.begin();
}