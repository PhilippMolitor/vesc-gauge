#include "VESC.h"

#include "buffer.h"
#include "crc.h"

#include <cstring>

namespace {
constexpr uint8_t PACKET_TYPE_8BIT = 2;
constexpr uint8_t PACKET_TYPE_16BIT = 3;
constexpr uint8_t PACKET_TYPE_24BIT = 4;
constexpr uint8_t PACKET_END_MARKER = 3;
constexpr size_t MIN_VALUES_PACKET_SIZE = 56;
constexpr size_t MIN_FW_VERSION_PACKET_SIZE = 3;
constexpr float SCALE_DUTY = 100000.0f;
constexpr float SCALE_CURRENT = 1000.0f;
constexpr float SCALE_HANDBRAKE = 1000.0f;
constexpr float SCALE_POSITION = 1000000.0f;
}

VESC::VESC() noexcept = default;

void VESC::begin(Stream* stream) noexcept
{
  m_stream = stream;
  m_rxReadPtr = 0;
  m_rxWritePtr = 0;
  m_bytesLeft = 0;
  m_rxBuffer.fill(0);
}

void VESC::setCanId(uint8_t canId) noexcept
{
  m_canId = canId;
}

void VESC::processAvailableData() noexcept
{
  if (!m_stream) {
    return;
  }

  // Process all available bytes
  while (m_stream->available()) {
    const auto byte = static_cast<uint8_t>(m_stream->read());
    processByte(byte);
  }
}

void VESC::setValuesCallback(vesc::ValuesCallback callback) noexcept
{
  m_valuesCallback = std::move(callback);
}

void VESC::setFirmwareVersionCallback(vesc::FirmwareVersionCallback callback) noexcept
{
  m_firmwareVersionCallback = std::move(callback);
}

void VESC::clearCallbacks() noexcept
{
  m_valuesCallback = nullptr;
  m_firmwareVersionCallback = nullptr;
}

vesc::Result VESC::requestFirmwareVersion() noexcept
{
  return sendPacket(vesc::PacketId::FW_VERSION);
}

vesc::Result VESC::requestValues() noexcept
{
  return sendPacket(vesc::PacketId::GET_VALUES);
}

vesc::Result VESC::sendKeepalive() noexcept
{
  return sendPacket(vesc::PacketId::ALIVE);
}

vesc::Result VESC::setDuty(float dutyCycle) noexcept
{
  std::array<uint8_t, 4> payload {};
  int32_t index = 0;
  buffer_append_int32(payload.data(), static_cast<int32_t>(dutyCycle * SCALE_DUTY), &index);

  return sendPacket(vesc::PacketId::SET_DUTY, payload.data(), index);
}

vesc::Result VESC::setCurrent(float current) noexcept
{
  std::array<uint8_t, 4> payload {};
  int32_t index = 0;
  buffer_append_int32(payload.data(), static_cast<int32_t>(current * SCALE_CURRENT), &index);

  return sendPacket(vesc::PacketId::SET_CURRENT, payload.data(), index);
}

vesc::Result VESC::setBrakeCurrent(float brakeCurrent) noexcept
{
  std::array<uint8_t, 4> payload {};
  int32_t index = 0;
  buffer_append_int32(payload.data(), static_cast<int32_t>(brakeCurrent * SCALE_CURRENT), &index);

  return sendPacket(vesc::PacketId::SET_CURRENT_BRAKE, payload.data(), index);
}

vesc::Result VESC::setRpm(float rpm) noexcept
{
  return setErpm(static_cast<int32_t>(rpm));
}

vesc::Result VESC::setErpm(int32_t erpm) noexcept
{
  std::array<uint8_t, 4> payload {};
  int32_t index = 0;
  buffer_append_int32(payload.data(), erpm, &index);

  return sendPacket(vesc::PacketId::SET_RPM, payload.data(), index);
}

vesc::Result VESC::setPosition(float position) noexcept
{
  std::array<uint8_t, 4> payload {};
  int32_t index = 0;
  buffer_append_int32(payload.data(), static_cast<int32_t>(position * SCALE_POSITION), &index);

  return sendPacket(vesc::PacketId::SET_POS, payload.data(), index);
}

vesc::Result VESC::setHandbrake(float brakeCurrent) noexcept
{
  std::array<uint8_t, 4> payload {};
  int32_t index = 0;
  buffer_append_float32(payload.data(), brakeCurrent, SCALE_HANDBRAKE, &index);

  return sendPacket(vesc::PacketId::SET_HANDBRAKE, payload.data(), index);
}

vesc::Result VESC::setNunchuck(const vesc::NunchuckValues& nunchuck) noexcept
{
  std::array<uint8_t, 11> payload {};
  int32_t index = 0;
  payload[index++] = nunchuck.js_x;
  payload[index++] = nunchuck.js_y;
  payload[index++] = nunchuck.bt_c;
  payload[index++] = nunchuck.bt_z;
  buffer_append_int16(payload.data(), nunchuck.acc_x, &index);
  buffer_append_int16(payload.data(), nunchuck.acc_y, &index);
  buffer_append_int16(payload.data(), nunchuck.acc_z, &index);

  if (index < static_cast<int32_t>(payload.size())) {
    // optional fields
    payload[index++] = nunchuck.rev_has_state;
    payload[index++] = nunchuck.is_rev;
  }

  return sendPacket(vesc::PacketId::SET_CHUCK_DATA, payload.data(), index);
}

void VESC::processByte(uint8_t rxData) noexcept
{
  auto dataLen = static_cast<uint32_t>(m_rxWritePtr - m_rxReadPtr);

  // buffer overflow
  if (dataLen >= vesc::RECEIVE_BUFFER_SIZE) {
    m_rxWritePtr = 0;
    m_rxReadPtr = 0;
    m_bytesLeft = 0;
    m_rxBuffer[m_rxWritePtr++] = rxData;
    return;
  }

  // shift buffer if we are out of space
  if (m_rxWritePtr >= vesc::RECEIVE_BUFFER_SIZE) {
    std::memmove(m_rxBuffer.data(),
        m_rxBuffer.data() + m_rxReadPtr,
        dataLen);

    m_rxReadPtr = 0;
    m_rxWritePtr = static_cast<uint16_t>(dataLen);
  }

  m_rxBuffer[m_rxWritePtr++] = rxData;
  dataLen++;

  if (m_bytesLeft > 1) {
    m_bytesLeft--;
    return;
  }

  // try decoding the packet at various offsets until it succeeds, or until we run out of data
  for (;;) {
    const int res = tryDecodePacket(m_rxBuffer.data() + m_rxReadPtr, dataLen, &m_bytesLeft);

    if (res == -2)
      break;

    if (res > 0) {
      dataLen -= static_cast<uint32_t>(res);
      m_rxReadPtr += static_cast<uint16_t>(res);
    } else if (res == -1) {
      m_rxReadPtr++;
      dataLen--;
    }
  }

  // nothing left, move pointers to avoid memmove
  if (dataLen == 0) {
    m_rxReadPtr = 0;
    m_rxWritePtr = 0;
  }
}

int VESC::tryDecodePacket(uint8_t* buffer, uint32_t inLen, int* bytesLeft) noexcept
{
  *bytesLeft = 0;

  if (inLen == 0) {
    *bytesLeft = 1;
    return -2;
  }

  const bool isLen8b = buffer[0] == PACKET_TYPE_8BIT;
  const auto dataStart = static_cast<uint32_t>(buffer[0]);

  const bool isLen16b = buffer[0] == PACKET_TYPE_16BIT;
  const bool isLen24b = buffer[0] == PACKET_TYPE_24BIT;

  if (!isLen8b && !isLen16b && !isLen24b)
    return -1;

  // Not enough data to determine length
  if (inLen < dataStart) {
    *bytesLeft = static_cast<int>(dataStart - inLen);
    return -2;
  }

  uint32_t len = 0;

  if (isLen8b) {
    len = static_cast<uint32_t>(buffer[1]);

    // No support for zero length packets
    if (len < 1) {
      return -1;
    }
  } else if (isLen16b) {
    len = (static_cast<uint32_t>(buffer[1]) << 8) | static_cast<uint32_t>(buffer[2]);

    // A shorter packet should use less length bytes
    if (len < 255) {
      return -1;
    }
  } else if (isLen24b) {
    len = (static_cast<uint32_t>(buffer[1]) << 16) | (static_cast<uint32_t>(buffer[2]) << 8) | static_cast<uint32_t>(buffer[3]);

    if (len < 65535)
      return -1;
  }

  // Too long packet
  if (len > vesc::RECEIVE_BUFFER_SIZE)
    return -1;

  // Need more data to determine rest of packet
  const auto totalPacketSize = len + dataStart + 3;
  if (inLen < totalPacketSize) {
    *bytesLeft = static_cast<int>(totalPacketSize - inLen);
    return -2;
  }

  // Invalid stop byte
  if (buffer[dataStart + len + 2] != PACKET_END_MARKER)
    return -1;

  const auto crcCalc = crc16(buffer + dataStart, len);
  const auto crcRx = static_cast<uint16_t>((static_cast<uint16_t>(buffer[dataStart + len]) << 8) | static_cast<uint16_t>(buffer[dataStart + len + 1]));

  if (crcCalc != crcRx)
    return -1;

  processPacket(buffer + dataStart, len);
  return static_cast<int>(totalPacketSize);
}

void VESC::processPacket(const uint8_t* data, uint32_t len) noexcept
{
  if (len < 1)
    return;

  const auto packetId = static_cast<vesc::PacketId>(data[0]);

  switch (packetId) {
  case vesc::PacketId::GET_VALUES:
    parseValuesPacket(data, len);
    break;

  case vesc::PacketId::FW_VERSION:
    parseFirmwareVersionPacket(data, len);
    break;

  default:
    break;
  }
}

vesc::Result VESC::sendPacket(vesc::PacketId packetId, const uint8_t* payload, size_t payloadLength) noexcept
{
  if (!m_stream)
    return vesc::Result::STREAM_ERROR;

  const auto commandLength = getCommandPayloadSize();
  const auto totalPayloadLength = commandLength + payloadLength;

  std::array<uint8_t, vesc::RECEIVE_BUFFER_SIZE> packet {};
  int32_t index = 0;

  if (totalPayloadLength <= 255) {
    packet[index++] = PACKET_TYPE_8BIT;
    packet[index++] = static_cast<uint8_t>(totalPayloadLength);
  } else if (totalPayloadLength <= 65535) {
    packet[index++] = PACKET_TYPE_16BIT;
    packet[index++] = static_cast<uint8_t>(totalPayloadLength >> 8);
    packet[index++] = static_cast<uint8_t>(totalPayloadLength & 0xFF);
  } else {
    packet[index++] = PACKET_TYPE_24BIT;
    packet[index++] = static_cast<uint8_t>(totalPayloadLength >> 16);
    packet[index++] = static_cast<uint8_t>((totalPayloadLength >> 8) & 0xFF);
    packet[index++] = static_cast<uint8_t>(totalPayloadLength & 0xFF);
  }
  const auto dataStartIndex = index;

  // add CAN forward if needed
  if (m_canId != 0) {
    packet[index++] = static_cast<uint8_t>(vesc::PacketId::FORWARD_CAN);
    packet[index++] = m_canId;
  }
  // add packet ID
  packet[index++] = static_cast<uint8_t>(packetId);
  // add payload
  if (payload && payloadLength > 0) {
    std::memcpy(packet.data() + index, payload, payloadLength);
    index += static_cast<int32_t>(payloadLength);
  }
  // CRC calculated on data starting from packet ID
  const auto crc = crc16(packet.data() + dataStartIndex, totalPayloadLength);
  // add CRC
  packet[index++] = static_cast<uint8_t>(crc >> 8);
  packet[index++] = static_cast<uint8_t>(crc & 0xFF);
  // add end marker
  packet[index++] = PACKET_END_MARKER;

  const auto written = m_stream->write(packet.data(), index);
  return (written == static_cast<size_t>(index)) ? vesc::Result::SUCCESS : vesc::Result::STREAM_ERROR;
}

size_t VESC::getCommandPayloadSize() const noexcept
{
  return (m_canId != 0) ? 3 : 1;
}

void VESC::parseValuesPacket(const uint8_t* data, size_t len) noexcept
{
  if (!m_valuesCallback || len < MIN_VALUES_PACKET_SIZE)
    return;

  vesc::MotorValues values {};
  int32_t idx = 1; // Skip packet ID

  // standard GET_VALUES response structure (mask = 0xFFFFFFFF)
  values.mosfetTemp = buffer_get_float16(data, 10.0, &idx); // 0: MOSFET temp
  values.motorTemp = buffer_get_float16(data, 10.0, &idx); // 1: Motor temp
  values.motorCurrentAverage = buffer_get_float32(data, 100.0, &idx); // 2: Motor current avg
  values.inputCurrentAverage = buffer_get_float32(data, 100.0, &idx); // 3: Input current avg
  values.id = buffer_get_float32(data, 100.0, &idx); // 4: ID current avg
  values.iq = buffer_get_float32(data, 100.0, &idx); // 5: IQ current avg
  values.dutyCycle = buffer_get_float16(data, 1000.0, &idx); // 6: Duty cycle
  values.motorRpm = buffer_get_float32(data, 1.0, &idx); // 7: RPM
  values.inputVoltage = buffer_get_float16(data, 10.0, &idx); // 8: Input voltage
  values.ampHoursConsumed = buffer_get_float32(data, 10000.0, &idx); // 9: Amp hours
  values.ampHoursCharged = buffer_get_float32(data, 10000.0, &idx); // 10: Amp hours charged
  values.wattHoursConsumed = buffer_get_float32(data, 10000.0, &idx); // 11: Watt hours
  values.wattHoursCharged = buffer_get_float32(data, 10000.0, &idx); // 12: Watt hours charged
  values.tachometer = buffer_get_int32(data, &idx); // 13: Tachometer
  values.tachometerAbs = buffer_get_int32(data, &idx); // 14: Tachometer abs
  values.faultCode = static_cast<vesc::FaultCode>(data[idx++]); // 15: Fault code
  values.pidPosition = buffer_get_float32(data, 1000000.0, &idx); // 16: PID position
  values.controllerId = data[idx++]; // 17: Controller ID
  // additional temperature readings if available (mask bit 18)
  if (idx + 6 <= static_cast<int32_t>(len)) {
    values.mosfetNtc1Temp = buffer_get_float16(data, 10.0, &idx); // 18: MOSFET temps
    values.mosfetNtc2Temp = buffer_get_float16(data, 10.0, &idx);
    values.mosfetNtc3Temp = buffer_get_float16(data, 10.0, &idx);
  }
  // VD/VQ averages if available (mask bits 19-20)
  if (idx + 8 <= static_cast<int32_t>(len)) {
    values.vd = buffer_get_float32(data, 1000.0, &idx); // 19: VD average
    values.vq = buffer_get_float32(data, 1000.0, &idx); // 20: VQ average
  }
  // status byte if available (mask bit 21)
  if (idx < static_cast<int32_t>(len)) {
    values.status = data[idx]; // 21: Status
  }

  m_valuesCallback(values);
}

void VESC::parseFirmwareVersionPacket(const uint8_t* data, size_t len) noexcept
{
  if (!m_firmwareVersionCallback || len < MIN_FW_VERSION_PACKET_SIZE)
    return;

  vesc::FirmwareVersion version {};
  version.major = data[1]; // Skip packet ID
  version.minor = data[2];

  m_firmwareVersionCallback(version);
}