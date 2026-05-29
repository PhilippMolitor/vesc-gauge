#pragma once

#include <PCF8575.h>
#include <Wire.h>

#include "settings.h"

class Vehicle {
  protected:
  PCF8575* m_pcf8575;

  public:
  class Lights {
public:
    enum class TurnSignal {
      OFF,
      LEFT,
      RIGHT,
    };

private:
    Vehicle& m_vehicle;
    bool m_stateHeadlight;
    bool m_stateTaillight;
    bool m_stateBrakeLight;
    TurnSignal m_stateTurnSignal;

public:
    Lights(Vehicle& vehicle);

    bool getHeadlight();
    void setHeadlight(bool state);
    bool getTailLight();
    void setTailLight(bool state);
    bool getBrakeLight();
    void setBrakeLight(bool state);
    TurnSignal getTurnSignal();
    void setTurnSignal(TurnSignal signal);
  };

  class Controls {
    enum class Update : uint8_t {
      NONE = 0b0, // unused
      PROFILE_CHANGE_UP = 0b1,
      PROFILE_CHANGE_DOWN = 0b10,
      LIGHTS_ON = 0b100,
      LIGHTS_OFF = 0b1000,
      TURNSIGNAL_LEFT = 0b10000,
      TURNSIGNAL_RIGHT = 0b100000,
    };

    using TurnSignalCallback = std::function<void(Lights::TurnSignal)>;
    using LightsCallback = std::function<void(bool)>;
    using ProfileChangeCallback = std::function<void(bool)>;

private:
    Vehicle& m_vehicle;
    uint8_t m_needsRead;
    uint8_t m_updates;

    ProfileChangeCallback m_profileChangeCallback;
    LightsCallback m_lightsCallback;
    TurnSignalCallback m_turnSignalCallback;

public:
    Controls(Vehicle& vehicle);

    void begin();
    void read();
    void update();

    void onProfileChange(ProfileChangeCallback callback);
    void onLightsSet(LightsCallback callback);
    void onTurnSignalSet(TurnSignalCallback callback);
  };

  Lights lights { *this };
  Controls controls { *this };

  Vehicle();

  void begin();
};
