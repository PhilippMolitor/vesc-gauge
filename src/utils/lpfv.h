#include <stdio.h>

class LPFilteredValue {
  private:
  float _value = 0.0f;
  float _weight = 0.95f;

  public:
  float update(float newValue)
  {
    _value = _value * _weight + newValue * (1.0f - _weight);
    return _value;
  }

  void setWeight(float w)
  {
    _weight = w < 0.0f ? 0.0f : (w > 1.0f ? 1.0f : w);
  }

  float getFloat() const
  {
    return _value;
  }

  int32_t getInt() const
  {
    return static_cast<int32_t>(_value + 0.5f);
  }
};
