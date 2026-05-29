#pragma once

#include "Stream.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <vector>

class TestStream : public Stream {

  private:
  std::deque<uint8_t> m_readBuffer;
  std::vector<uint8_t> m_writeBuffer;

  class Test {
public:
    Test(TestStream& parent)
        : m_parent(parent)
    {
    }

    void addReadBufferData(const uint8_t* data, size_t size)
    {
      for (size_t i = 0; i < size; ++i)
        m_parent.m_readBuffer.push_back(data[i]);
    }

    void addReadBufferData(const std::vector<uint8_t>& data)
    {
      for (uint8_t byte : data)
        m_parent.m_readBuffer.push_back(byte);
    }

    void clearReadBuffer()
    {
      m_parent.m_readBuffer.clear();
    }

    std::vector<uint8_t> getWriteBufferData() const
    {
      return m_parent.m_writeBuffer;
    }

    void clearWriteBuffer()
    {
      m_parent.m_writeBuffer.clear();
    }

    size_t getWriteBufferSize() const
    {
      return m_parent.m_writeBuffer.size();
    }

    bool isWriteBufferEmpty() const
    {
      return m_parent.m_writeBuffer.empty();
    }

private:
    TestStream& m_parent;
  };

  public:
  TestStream() = default;
  ~TestStream() override = default;

  // Stream interface implementation
  int available() override
  {
    return static_cast<int>(m_readBuffer.size());
  }

  int read() override
  {
    if (m_readBuffer.empty())
      return -1;
    uint8_t byte = m_readBuffer.front();
    m_readBuffer.pop_front();
    return byte;
  }

  int peek() override
  {
    if (m_readBuffer.empty())
      return -1;
    return m_readBuffer.front();
  }

  void flush() override
  {
    // no-op
  }

  size_t write(uint8_t data) override
  {
    m_writeBuffer.push_back(data);
    return 1;
  }

  size_t write(const uint8_t* buffer, size_t size) override
  {
    if (!buffer || size == 0)
      return 0;
    for (size_t i = 0; i < size; ++i)
      m_writeBuffer.push_back(buffer[i]);
    return size;
  }

  Test test { *this };
};
