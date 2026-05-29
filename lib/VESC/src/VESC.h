#pragma once

#include <array>
#include <cstdint>
#include <functional>

#include <Stream.h>

namespace vesc {

constexpr size_t RECEIVE_BUFFER_SIZE = 256u;
constexpr uint32_t DEFAULT_TIMEOUT = 100u;

enum class Result : uint8_t {
  SUCCESS = 0,
  TIMEOUT,
  INVALID_PACKET,
  CRC_ERROR,
  WRONG_PACKET_ID,
  STREAM_ERROR,
  BUFFER_OVERFLOW
};

enum class PacketId : uint8_t {
  FW_VERSION = 0,
  JUMP_TO_BOOTLOADER = 1,
  ERASE_NEW_APP = 2,
  WRITE_NEW_APP_DATA = 3,
  GET_VALUES = 4,
  SET_DUTY = 5,
  SET_CURRENT = 6,
  SET_CURRENT_BRAKE = 7,
  SET_RPM = 8,
  SET_POS = 9,
  SET_HANDBRAKE = 10,
  SET_DETECT = 11,
  SET_SERVO_POS = 12,
  SET_MCCONF = 13,
  GET_MCCONF = 14,
  GET_MCCONF_DEFAULT = 15,
  SET_APPCONF = 16,
  GET_APPCONF = 17,
  GET_APPCONF_DEFAULT = 18,
  SAMPLE_PRINT = 19,
  TERMINAL_CMD = 20,
  PRINT = 21,
  ROTOR_POSITION = 22,
  EXPERIMENT_SAMPLE = 23,
  DETECT_MOTOR_PARAM = 24,
  DETECT_MOTOR_R_L = 25,
  DETECT_MOTOR_FLUX_LINKAGE = 26,
  DETECT_ENCODER = 27,
  DETECT_HALL_FOC = 28,
  REBOOT = 29,
  ALIVE = 30,
  GET_DECODED_PPM = 31,
  GET_DECODED_ADC = 32,
  GET_DECODED_CHUK = 33,
  FORWARD_CAN = 34,
  SET_CHUCK_DATA = 35,
  CUSTOM_APP_DATA = 36,
  NRF_START_PAIRING = 37,

  // BMS commands
  BMS_GET_VALUES = 96,
  BMS_SET_CHARGE_ALLOWED = 97,
  BMS_SET_BALANCE_OVERRIDE = 98,
  BMS_RESET_COUNTERS = 99,
  BMS_FORCE_BALANCE = 100,
  BMS_ZERO_CURRENT_OFFSET = 101,

  SET_ODOMETER = 110,

  // Power switch commands
  PSW_GET_STATUS = 111,
  PSW_SWITCH = 112,

  BMS_FWD_CAN_RX = 113,
  BMS_HW_DATA = 114,
  GET_BATTERY_CUT = 115,
  BM_HALT_REQ = 116,

  GET_STATS = 128,
  RESET_STATS = 129,

  SHUTDOWN = 156,
};

enum class FaultCode : uint8_t {
  NONE = 0,
  OVER_VOLTAGE,
  UNDER_VOLTAGE,
  DRV,
  ABS_OVER_CURRENT,
  OVER_TEMP_FET,
  OVER_TEMP_MOTOR,
  GATE_DRIVER_OVER_VOLTAGE,
  GATE_DRIVER_UNDER_VOLTAGE,
  MCU_UNDER_VOLTAGE,
  BOOTING_FROM_WATCHDOG_RESET,
  ENCODER_SPI,
  ENCODER_SINCOS_BELOW_MIN_AMPLITUDE,
  ENCODER_SINCOS_ABOVE_MAX_AMPLITUDE,
  FLASH_CORRUPTION,
  HIGH_OFFSET_CURRENT_SENSOR_1,
  HIGH_OFFSET_CURRENT_SENSOR_2,
  HIGH_OFFSET_CURRENT_SENSOR_3,
  UNBALANCED_CURRENTS,
  BRK,
  RESOLVER_LOT,
  RESOLVER_DOS,
  RESOLVER_LOS,
  FLASH_CORRUPTION_APP_CFG,
  FLASH_CORRUPTION_MC_CFG,
  ENCODER_NO_MAGNET,
  ENCODER_MAGNET_TOO_STRONG,
  PHASE_FILTER,
  ENCODER_FAULT,
  LV_OUTPUT_FAULT,
};

struct MotorValues {
  float mosfetTemp { 0.0f };
  float motorTemp { 0.0f };
  float motorCurrentAverage { 0.0f };
  float inputCurrentAverage { 0.0f };
  float id { 0.0f };
  float iq { 0.0f };
  float dutyCycle { 0.0f };
  float motorRpm { 0.0f };
  float inputVoltage { 0.0f };
  float ampHoursConsumed { 0.0f };
  float ampHoursCharged { 0.0f };
  float wattHoursConsumed { 0.0f };
  float wattHoursCharged { 0.0f };
  int32_t tachometer { 0 };
  int32_t tachometerAbs { 0 };
  FaultCode faultCode { FaultCode::NONE };
  float pidPosition { 0.0f };
  uint8_t controllerId { 0 };
  float mosfetNtc1Temp { 0.0f };
  float mosfetNtc2Temp { 0.0f };
  float mosfetNtc3Temp { 0.0f };
  float vd { 0.0f };
  float vq { 0.0f };
  uint8_t status { 0 };
};

struct NunchuckValues {
  uint8_t js_x { 127 };
  uint8_t js_y { 127 };
  uint8_t bt_c { 0 };
  uint8_t bt_z { 0 };
  int16_t acc_x { 0 };
  int16_t acc_y { 0 };
  int16_t acc_z { 0 };
  uint8_t rev_has_state { 0 };
  uint8_t is_rev { 0 };
};

struct FirmwareVersion {
  uint8_t major { 0 };
  uint8_t minor { 0 };
};

using ValuesCallback = std::function<void(const MotorValues& values)>;
using FirmwareVersionCallback = std::function<void(const FirmwareVersion& version)>;

}

class VESC {
  public:
  VESC() noexcept;
  ~VESC() = default;

  // Non-copyable, non-movable for safety
  VESC(const VESC&) = delete;
  VESC& operator=(const VESC&) = delete;
  VESC(VESC&&) = delete;
  VESC& operator=(VESC&&) = delete;

  /**
   * @brief      Initialize the VESC instance
   * @param      stream  The stream to use for communication
   */
  void begin(Stream* stream) noexcept;

  /**
   * @brief      Set the CAN ID of the controller that will receive the commands
   * @param      canId  The target controller's CAN ID (`0` for no CAN forwarding)
   */
  void setCanId(uint8_t canId) noexcept;

  /**
   * @brief      Process available data from the stream. Call this regularly in your main loop.
   */
  void processAvailableData() noexcept;

  /**
   * @brief      Register a callback for motor values
   * @param      callback  Function to call when GET_VALUES response is received
   */
  void setValuesCallback(vesc::ValuesCallback callback) noexcept;

  /**
   * @brief      Register a callback for firmware version
   * @param      callback  Function to call when FW_VERSION response is received
   */
  void setFirmwareVersionCallback(vesc::FirmwareVersionCallback callback) noexcept;

  /**
   * @brief      Remove all registered callbacks
   */
  void clearCallbacks() noexcept;

  /**
   * @brief      Request firmware version (response via callback)
   */
  vesc::Result requestFirmwareVersion() noexcept;

  /**
   * @brief      Request motor values (response via callback)
   */
  vesc::Result requestValues() noexcept;

  /**
   * @brief      Send keepalive packet
   */
  vesc::Result sendKeepalive() noexcept;

  /**
   * @brief      Set motor duty cycle
   * @param      dutyCycle The desired duty cycle (-1.0 to 1.0)
   */
  vesc::Result setDuty(float dutyCycle) noexcept;

  /**
   * @brief      Set motor current
   * @param      current The desired current in Amps
   */
  vesc::Result setCurrent(float current) noexcept;

  /**
   * @brief      Set brake current
   * @param      brakeCurrent The desired brake current in Amps
   */
  vesc::Result setBrakeCurrent(float brakeCurrent) noexcept;

  /**
   * @brief      Set motor RPM (actually eRPM)
   * @param      rpm The desired RPM
   */
  vesc::Result setRpm(float rpm) noexcept;

  /**
   * @brief      Set motor eRPM directly
   * @param      erpm The desired eRPM
   */
  vesc::Result setErpm(int32_t erpm) noexcept;

  /**
   * @brief      Set motor position
   * @param      position The desired position
   */
  vesc::Result setPosition(float position) noexcept;

  /**
   * @brief      Set handbrake current
   * @param      brakeCurrent The desired handbrake current
   */
  vesc::Result setHandbrake(float brakeCurrent) noexcept;

  /**
   * @brief      Send nunchuck data
   * @param      nunchuck The nunchuck data to send
   */
  vesc::Result setNunchuck(const vesc::NunchuckValues& nunchuck) noexcept;

  private:
  // Stream and configuration
  Stream* m_stream { nullptr };
  uint8_t m_canId { 0 };

  // Packet processing state (similar to VESC firmware packet.c)
  uint16_t m_rxReadPtr { 0 };
  uint16_t m_rxWritePtr { 0 };
  int m_bytesLeft { 0 };
  std::array<uint8_t, vesc::RECEIVE_BUFFER_SIZE> m_rxBuffer {};

  // Type-safe callback storage
  vesc::ValuesCallback m_valuesCallback;
  vesc::FirmwareVersionCallback m_firmwareVersionCallback;

  /**
   * @brief      Process a single byte (similar to VESC packet_process_byte)
   * @param      rxData The byte to process
   */
  void processByte(uint8_t rxData) noexcept;

  /**
   * @brief      Try to decode a packet from the buffer
   * @param      buffer    The buffer to decode from
   * @param      inLen     Length of buffer
   * @param      bytesLeft Output: how many more bytes are needed
   * @return     >0: Success, bytes consumed; -1: Invalid; -2: Need more data
   */
  int tryDecodePacket(uint8_t* buffer, uint32_t inLen, int* bytesLeft) noexcept;

  /**
   * @brief      Process a complete decoded packet
   * @param      data The packet data (including packet ID)
   * @param      len  Length of packet data
   */
  void processPacket(const uint8_t* data, uint32_t len) noexcept;

  /**
   * @brief      Send a raw packet
   * @param      packetId      The packet ID
   * @param      payload       Optional payload data
   * @param      payloadLength Length of payload
   */
  vesc::Result sendPacket(vesc::PacketId packetId, const uint8_t* payload = nullptr, size_t payloadLength = 0) noexcept;

  /**
   * @brief      Get the command payload size (packet ID + optional CAN forward)
   */
  size_t getCommandPayloadSize() const noexcept;

  /**
   * @brief      Parse GET_VALUES response
   * @param      data Packet data (starting with packet ID)
   * @param      len  Length of packet data
   */
  void parseValuesPacket(const uint8_t* data, size_t len) noexcept;

  /**
   * @brief      Parse FW_VERSION response
   * @param      data Packet data (starting with packet ID)
   * @param      len  Length of packet data
   */
  void parseFirmwareVersionPacket(const uint8_t* data, size_t len) noexcept;
};
