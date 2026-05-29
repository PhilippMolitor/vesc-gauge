#include <cstring>
#include <vector>

#include <unity.h>

#include "VESC.h"

#include "buffer.h"
#include "crc.h"

#include "include/TestStream.h"

namespace TestHelpers {

std::vector<uint8_t> createVescPacket(uint8_t packetId, const std::vector<uint8_t>& payload = {})
{
  std::vector<uint8_t> packet;

  // Calculate total payload size (packet ID + payload)
  size_t totalPayloadSize = 1 + payload.size();

  // Add start byte and length based on payload size
  if (totalPayloadSize <= 255) {
    packet.push_back(2); // PACKET_TYPE_8BIT
    packet.push_back(static_cast<uint8_t>(totalPayloadSize));
  } else if (totalPayloadSize <= 65535) {
    packet.push_back(3); // PACKET_TYPE_16BIT
    packet.push_back(static_cast<uint8_t>(totalPayloadSize >> 8));
    packet.push_back(static_cast<uint8_t>(totalPayloadSize & 0xFF));
  } else {
    packet.push_back(4); // PACKET_TYPE_24BIT
    packet.push_back(static_cast<uint8_t>(totalPayloadSize >> 16));
    packet.push_back(static_cast<uint8_t>((totalPayloadSize >> 8) & 0xFF));
    packet.push_back(static_cast<uint8_t>(totalPayloadSize & 0xFF));
  }

  // Add packet ID
  packet.push_back(packetId);
  // Add payload
  for (uint8_t byte : payload)
    packet.push_back(byte);

  // Calculate CRC on packet ID + payload using VESC library function
  uint16_t crcValue = crc16(packet.data() + (packet.size() - totalPayloadSize), totalPayloadSize);

  // Add CRC (big endian)
  packet.push_back(static_cast<uint8_t>(crcValue >> 8));
  packet.push_back(static_cast<uint8_t>(crcValue & 0xFF));

  // Add end marker
  packet.push_back(3); // PACKET_END_MARKER

  return packet;
}

// Create firmware version response with explicit values
std::vector<uint8_t> createFirmwareVersionResponse(uint8_t major, uint8_t minor)
{
  return createVescPacket(0, { major, minor }); // FW_VERSION = 0
}

// Create GET_VALUES response with explicit test values
std::vector<uint8_t> createGetValuesResponse(float mosfetTemp, float motorTemp,
    float motorCurrent, float inputCurrent)
{
  std::vector<uint8_t> payload(64); // Pre-allocate enough space for GET_VALUES response
  int32_t index = 0;

  // Build GET_VALUES response with explicit values using VESC library functions
  buffer_append_float16(payload.data(), mosfetTemp, 10.0f, &index); // MOSFET temp
  buffer_append_float16(payload.data(), motorTemp, 10.0f, &index); // Motor temp
  buffer_append_float32(payload.data(), motorCurrent, 100.0f, &index); // Motor current avg
  buffer_append_float32(payload.data(), inputCurrent, 100.0f, &index); // Input current avg

  // Add minimal required fields to meet packet size requirements
  // (id, iq, duty cycle, RPM, voltage, etc.) - using zeros for simplicity
  while (index < 56)
    payload[index++] = 0x00;
  payload.resize(index);

  return createVescPacket(4, payload); // GET_VALUES = 4
}

}

// Global test variables
static bool g_valuesCallbackTriggered = false;
static bool g_firmwareCallbackTriggered = false;
static vesc::MotorValues g_receivedValues;
static vesc::FirmwareVersion g_receivedFirmware;

// Test callbacks
void testValuesCallback(const vesc::MotorValues& values)
{
  g_valuesCallbackTriggered = true;
  g_receivedValues = values;
}

void testFirmwareCallback(const vesc::FirmwareVersion& version)
{
  g_firmwareCallbackTriggered = true;
  g_receivedFirmware = version;
}

void setUp(void)
{
  // Reset global test state before each test
  g_valuesCallbackTriggered = false;
  g_firmwareCallbackTriggered = false;
  g_receivedValues = {};
  g_receivedFirmware = {};
}

void tearDown(void)
{
  // Clean up after each test
}

void test_vesc_instantiation(void)
{
  VESC vesc;
  TEST_ASSERT_TRUE(true); // Just verify instantiation doesn't crash
}

void test_vesc_basic_configuration(void)
{
  VESC vesc;
  TestStream testStream;

  // Test begin
  vesc.begin(&testStream);

  // Test CAN ID setting
  vesc.setCanId(0);
  vesc.setCanId(1);
  vesc.setCanId(255);

  // Test callback setting
  vesc.setValuesCallback(testValuesCallback);
  vesc.setFirmwareVersionCallback(testFirmwareCallback);
  vesc.clearCallbacks();

  TEST_ASSERT_TRUE(true); // Verify no crashes
}

void test_vesc_packet_sending(void)
{
  VESC vesc;
  TestStream testStream;

  vesc.begin(&testStream);

  // Test firmware version request
  vesc::Result result = vesc.requestFirmwareVersion();
  TEST_ASSERT_EQUAL(vesc::Result::SUCCESS, result);

  // Verify packet was sent
  TEST_ASSERT_FALSE(testStream.test.isWriteBufferEmpty());

  auto writtenData = testStream.test.getWriteBufferData();
  TEST_ASSERT_TRUE(writtenData.size() >= 6); // Minimum packet size

  // Verify packet structure: [start_byte, length, packet_id, crc_high, crc_low, end_marker]
  TEST_ASSERT_EQUAL(2, writtenData[0]); // PACKET_TYPE_8BIT
  TEST_ASSERT_EQUAL(1, writtenData[1]); // Length = 1 (just packet ID)
  TEST_ASSERT_EQUAL(0, writtenData[2]); // FW_VERSION packet ID
  TEST_ASSERT_EQUAL(3, writtenData[writtenData.size() - 1]); // End marker

  testStream.test.clearWriteBuffer();

  // Test GET_VALUES request
  result = vesc.requestValues();
  TEST_ASSERT_EQUAL(vesc::Result::SUCCESS, result);
  TEST_ASSERT_FALSE(testStream.test.isWriteBufferEmpty());

  writtenData = testStream.test.getWriteBufferData();
  TEST_ASSERT_EQUAL(2, writtenData[0]); // PACKET_TYPE_8BIT
  TEST_ASSERT_EQUAL(1, writtenData[1]); // Length = 1
  TEST_ASSERT_EQUAL(4, writtenData[2]); // GET_VALUES packet ID
}

void test_vesc_duty_cycle_command(void)
{
  VESC vesc;
  TestStream testStream;

  vesc.begin(&testStream);

  // Test SET_DUTY command with 50% duty cycle
  vesc::Result result = vesc.setDuty(0.5f);
  TEST_ASSERT_EQUAL(vesc::Result::SUCCESS, result);

  auto writtenData = testStream.test.getWriteBufferData();
  TEST_ASSERT_TRUE(writtenData.size() >= 10); // Header + 4 bytes payload + CRC + end

  // Verify packet structure
  TEST_ASSERT_EQUAL(2, writtenData[0]); // PACKET_TYPE_8BIT
  TEST_ASSERT_EQUAL(5, writtenData[1]); // Length = 5 (packet ID + 4 bytes payload)
  TEST_ASSERT_EQUAL(5, writtenData[2]); // SET_DUTY packet ID

  // Verify duty cycle payload (0.5 * 100000 = 50000 = 0x0000C350)
  // Payload should be big-endian int32
  TEST_ASSERT_EQUAL(0x00, writtenData[3]);
  TEST_ASSERT_EQUAL(0x00, writtenData[4]);
  TEST_ASSERT_EQUAL(0xC3, writtenData[5]);
  TEST_ASSERT_EQUAL(0x50, writtenData[6]);
}

void test_vesc_current_commands(void)
{
  VESC vesc;
  TestStream testStream;

  vesc.begin(&testStream);

  // Test SET_CURRENT command
  vesc::Result result = vesc.setCurrent(10.0f);
  TEST_ASSERT_EQUAL(vesc::Result::SUCCESS, result);

  auto writtenData = testStream.test.getWriteBufferData();
  TEST_ASSERT_EQUAL(6, writtenData[2]); // SET_CURRENT packet ID

  testStream.test.clearWriteBuffer();

  // Test SET_CURRENT_BRAKE command
  result = vesc.setBrakeCurrent(5.0f);
  TEST_ASSERT_EQUAL(vesc::Result::SUCCESS, result);

  writtenData = testStream.test.getWriteBufferData();
  TEST_ASSERT_EQUAL(7, writtenData[2]); // SET_CURRENT_BRAKE packet ID
}

void test_vesc_firmware_version_response(void)
{
  VESC vesc;
  TestStream testStream;

  vesc.begin(&testStream);
  vesc.setFirmwareVersionCallback(testFirmwareCallback);

  // Simulate firmware version response: v5.03
  auto responsePacket = TestHelpers::createFirmwareVersionResponse(5, 3);
  testStream.test.addReadBufferData(responsePacket);

  // Process the response
  vesc.processAvailableData();

  // Verify callback was triggered
  TEST_ASSERT_TRUE(g_firmwareCallbackTriggered);
  TEST_ASSERT_EQUAL(5, g_receivedFirmware.major);
  TEST_ASSERT_EQUAL(3, g_receivedFirmware.minor);
}

void test_vesc_get_values_response(void)
{
  VESC vesc;
  TestStream testStream;

  vesc.begin(&testStream);
  vesc.setValuesCallback(testValuesCallback);

  // Simulate GET_VALUES response with explicit test values
  const float testMosfetTemp = 45.5f; // °C
  const float testMotorTemp = 38.2f; // °C
  const float testMotorCurrent = 12.34f; // A
  const float testInputCurrent = 10.5f; // A

  auto responsePacket = TestHelpers::createGetValuesResponse(
      testMosfetTemp, testMotorTemp, testMotorCurrent, testInputCurrent);
  testStream.test.addReadBufferData(responsePacket);

  // Process the response
  vesc.processAvailableData();

  // Verify callback was triggered
  TEST_ASSERT_TRUE(g_valuesCallbackTriggered);

  // Verify the exact values we sent
  TEST_ASSERT_FLOAT_WITHIN(0.1f, testMosfetTemp, g_receivedValues.mosfetTemp);
  TEST_ASSERT_FLOAT_WITHIN(0.1f, testMotorTemp, g_receivedValues.motorTemp);
  TEST_ASSERT_FLOAT_WITHIN(0.01f, testMotorCurrent, g_receivedValues.motorCurrentAverage);
  TEST_ASSERT_FLOAT_WITHIN(0.01f, testInputCurrent, g_receivedValues.inputCurrentAverage);
  TEST_ASSERT_EQUAL(vesc::FaultCode::NONE, g_receivedValues.faultCode);
}

void test_vesc_can_forwarding(void)
{
  VESC vesc;
  TestStream testStream;

  vesc.begin(&testStream);
  vesc.setCanId(123); // Set CAN forwarding ID

  // Send a command - should include CAN forwarding header
  vesc::Result result = vesc.requestFirmwareVersion();
  TEST_ASSERT_EQUAL(vesc::Result::SUCCESS, result);

  auto writtenData = testStream.test.getWriteBufferData();

  // With CAN forwarding, packet should be longer
  TEST_ASSERT_TRUE(writtenData.size() >= 8);
  TEST_ASSERT_EQUAL(2, writtenData[0]); // PACKET_TYPE_8BIT
  TEST_ASSERT_EQUAL(3, writtenData[1]); // Length = 3 (CAN forward + packet ID)
  TEST_ASSERT_EQUAL(34, writtenData[2]); // FORWARD_CAN packet ID
  TEST_ASSERT_EQUAL(123, writtenData[3]); // CAN ID
  TEST_ASSERT_EQUAL(0, writtenData[4]); // FW_VERSION packet ID
}

void test_vesc_packet_processing_edge_cases(void)
{
  VESC vesc;
  TestStream testStream;

  vesc.begin(&testStream);

  // Test with empty stream
  vesc.processAvailableData();
  TEST_ASSERT_TRUE(true); // Should not crash

  // Test with incomplete packet
  std::vector<uint8_t> incompletePacket = { 2, 5 }; // Start of packet but incomplete
  testStream.test.addReadBufferData(incompletePacket);
  vesc.processAvailableData();
  TEST_ASSERT_TRUE(true); // Should not crash

  // Test with invalid start byte
  testStream.test.clearReadBuffer();
  std::vector<uint8_t> invalidPacket = { 255, 1, 0, 0, 0, 3 }; // Invalid start byte
  testStream.test.addReadBufferData(invalidPacket);
  vesc.processAvailableData();
  TEST_ASSERT_TRUE(true); // Should handle gracefully
}

void test_vesc_error_conditions(void)
{
  VESC vesc;

  // Test operations without stream
  vesc::Result result = vesc.requestFirmwareVersion();
  TEST_ASSERT_EQUAL(vesc::Result::STREAM_ERROR, result);

  result = vesc.requestValues();
  TEST_ASSERT_EQUAL(vesc::Result::STREAM_ERROR, result);

  result = vesc.setDuty(0.5f);
  TEST_ASSERT_EQUAL(vesc::Result::STREAM_ERROR, result);
}

void test_vesc_nunchuck_data(void)
{
  VESC vesc;
  TestStream testStream;

  vesc.begin(&testStream);

  // Create nunchuck data with explicit test values
  vesc::NunchuckValues nunchuck;
  nunchuck.js_x = 100;
  nunchuck.js_y = 150;
  nunchuck.bt_c = 1;
  nunchuck.bt_z = 0;
  nunchuck.acc_x = 500;
  nunchuck.acc_y = -200;
  nunchuck.acc_z = 1000;

  vesc::Result result = vesc.setNunchuck(nunchuck);
  TEST_ASSERT_EQUAL(vesc::Result::SUCCESS, result);

  auto writtenData = testStream.test.getWriteBufferData();
  TEST_ASSERT_EQUAL(35, writtenData[2]); // SET_CHUCK_DATA packet ID

  // Verify the exact payload data we sent
  TEST_ASSERT_EQUAL(100, writtenData[3]); // js_x
  TEST_ASSERT_EQUAL(150, writtenData[4]); // js_y
  TEST_ASSERT_EQUAL(1, writtenData[5]); // bt_c
}

void test_vesc_custom_response_parsing(void)
{
  VESC vesc;
  TestStream testStream;

  vesc.begin(&testStream);
  vesc.setValuesCallback(testValuesCallback);

  // Test with extreme values to verify proper scaling and parsing
  const float extremeMosfetTemp = 85.0f; // High temp
  const float extremeMotorTemp = -10.5f; // Negative temp
  const float highCurrent = 99.99f; // High current
  const float lowCurrent = 0.01f; // Very low current

  auto extremePacket = TestHelpers::createGetValuesResponse(
      extremeMosfetTemp, extremeMotorTemp, highCurrent, lowCurrent);
  testStream.test.addReadBufferData(extremePacket);

  vesc.processAvailableData();

  // Verify extreme values are parsed correctly
  TEST_ASSERT_TRUE(g_valuesCallbackTriggered);
  TEST_ASSERT_FLOAT_WITHIN(0.1f, extremeMosfetTemp, g_receivedValues.mosfetTemp);
  TEST_ASSERT_FLOAT_WITHIN(0.1f, extremeMotorTemp, g_receivedValues.motorTemp);
  TEST_ASSERT_FLOAT_WITHIN(0.01f, highCurrent, g_receivedValues.motorCurrentAverage);
  TEST_ASSERT_FLOAT_WITHIN(0.01f, lowCurrent, g_receivedValues.inputCurrentAverage);
}

void test_vesc_firmware_version_edge_cases(void)
{
  VESC vesc;
  TestStream testStream;

  vesc.begin(&testStream);
  vesc.setFirmwareVersionCallback(testFirmwareCallback);

  // Test firmware version edge cases - very high version numbers
  const uint8_t majorVersion = 255;
  const uint8_t minorVersion = 99;

  auto versionPacket = TestHelpers::createFirmwareVersionResponse(majorVersion, minorVersion);
  testStream.test.addReadBufferData(versionPacket);

  vesc.processAvailableData();

  TEST_ASSERT_TRUE(g_firmwareCallbackTriggered);
  TEST_ASSERT_EQUAL(majorVersion, g_receivedFirmware.major);
  TEST_ASSERT_EQUAL(minorVersion, g_receivedFirmware.minor);
}

int main(int, char**)
{
  UNITY_BEGIN();

  RUN_TEST(test_vesc_instantiation);
  RUN_TEST(test_vesc_basic_configuration);
  RUN_TEST(test_vesc_packet_sending);
  RUN_TEST(test_vesc_duty_cycle_command);
  RUN_TEST(test_vesc_current_commands);
  RUN_TEST(test_vesc_firmware_version_response);
  RUN_TEST(test_vesc_get_values_response);
  RUN_TEST(test_vesc_can_forwarding);
  RUN_TEST(test_vesc_packet_processing_edge_cases);
  RUN_TEST(test_vesc_error_conditions);
  RUN_TEST(test_vesc_nunchuck_data);
  RUN_TEST(test_vesc_custom_response_parsing);
  RUN_TEST(test_vesc_firmware_version_edge_cases);

  return UNITY_END();
}