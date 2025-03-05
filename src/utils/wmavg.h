#pragma once

#include <inttypes.h>

template <size_t window_ms, size_t dt_ms>
class WeightedMovingAverage {
  static_assert(window_ms % dt_ms == 0, "window_ms must be a multiple of dt_ms.");

  private:
  static constexpr size_t window_size = window_ms / dt_ms;
  float _weight_accel;
  float _weight_decel;
  float _threshold_small_change;

  float _buffer[window_size];
  float _filtered_value;
  size_t _index;

  public:
  WeightedMovingAverage(
      float riseNewWeight = 0.9f,
      float fallNewWeight = 0.2f,
      float smallChangeThreshold = 2.0f)
      : _weight_accel(riseNewWeight)
      , _weight_decel(fallNewWeight)
      , _threshold_small_change(smallChangeThreshold)
      , _filtered_value(0)
      , _index(0)
  {
    for (size_t i = 0; i < window_size; i++)
      _buffer[i] = _filtered_value;
  }

  float update(float newValue)
  {
    _buffer[_index] = newValue;
    size_t index_current = _index;
    _index = (_index + 1) % window_size;

    float min_val = _buffer[0];
    float max_val = _buffer[0];
    for (size_t i = 1; i < window_size; i++) {
      if (_buffer[i] < min_val)
        min_val = _buffer[i];
      if (_buffer[i] > max_val)
        max_val = _buffer[i];
    }

    float window_range = max_val - min_val;
    float factor = (window_range < _threshold_small_change) ? (window_range / _threshold_small_change) : 1.0f;
    float baseNewWeight = (_buffer[index_current] >= _filtered_value) ? _weight_accel : _weight_decel;
    float newWeight = baseNewWeight * factor;

    float weightedSum = 0.0f;
    for (std::size_t i = 0; i < window_size; i++)
      if (i == index_current)
        weightedSum += _buffer[i] * newWeight;
      else
        weightedSum += _buffer[i] * ((1.0f - newWeight) / (window_size - 1));
    _filtered_value = weightedSum;

    return _filtered_value;
  }

  // Return the current filtered value.
  float filteredValue() const
  {
    return _filtered_value;
  }

  float lastValue() const
  {
    return _buffer[(_index + window_size - 1) % window_size];
  }

  // Optional setters to adjust parameters at runtime.
  void setRiseNewWeight(float w) { _weight_accel = w; }
  void setFallNewWeight(float w) { _weight_decel = w; }
  void setSmallChangeThreshold(float t) { _threshold_small_change = t; }
};
