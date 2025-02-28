#pragma once

#include <math.h>

#include <Arduino.h>

class TimeBasedLPF {
  private:
  float _filteredValue;
  float _tauUp;
  float _tauDown;
  unsigned long _lastUpdate;

  public:
  TimeBasedLPF(float initValue = 0.0f, float tauUp = 0.8f, float tauDown = 0.3f)
      : _filteredValue(initValue)
      , _tauUp(tauUp)
      , _tauDown(tauDown)
      , _lastUpdate(millis())
  {
  }

  void setTauAccel(float tau) { _tauUp = tau < 0 ? 0 : tau; }

  void setTauDecel(float tau) { _tauDown = tau < 0 ? 0 : tau; }

  float update(float newValue)
  {
    unsigned long now = millis();
    float dt = (now - _lastUpdate) / 1000.0f;
    _lastUpdate = now;

    float currentTau = (newValue > _filteredValue) ? _tauUp : _tauDown;
    float alpha = 1.0f - expf(-dt / currentTau);

    _filteredValue = _filteredValue + alpha * (newValue - _filteredValue);
    return _filteredValue;
  }

  float value() const { return _filteredValue; }
};