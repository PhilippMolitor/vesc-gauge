#pragma once

#include <inttypes.h>

template <typename T, size_t S>
class FastFIFO {
  private:
  T m_ringBuf[S];
  size_t m_headIndex {};
  size_t m_tailIndex {};

  public:
  FastFIFO()
      : m_headIndex { 0 }
      , m_tailIndex { 0 } { };

  bool enqueue(T value)
  {
    size_t nextIndex = (m_headIndex + 1) % S;

    // set value and new head index
    m_ringBuf[m_headIndex] = value;
    m_headIndex = nextIndex;

    return true;
  };

  bool dequeue(T* out)
  {
    // check if buffer is empty
    if (m_headIndex == m_tailIndex)
      return false;

    *out = m_ringBuf[m_tailIndex];

    // increment tail so this value is removed
    m_tailIndex = (m_tailIndex + 1) % S;

    return true;
  };

  void clear()
  {
    // reset head and tail indexes
    m_headIndex = 0;
    m_tailIndex = 0;
  };

  bool isEmpty() { return m_headIndex == m_tailIndex; };

  bool isFull() { return (m_headIndex + 1) % S == m_tailIndex; };

  size_t depth() { return (S + m_headIndex - m_tailIndex) % S; };
};
