#ifndef BELMOOR_AM2320_HPP_
#define BELMOOR_AM2320_HPP_ 1

#include "pack.hpp"

#include <i2c.h>

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>

namespace belmoor {

  enum class AM2320_command_id : uint8_t {
    ReadRegister = 3,
  };

  enum class AM2320_register : uint8_t {
    Humidity,
    Temperature = 2,
  };

  struct AM2320_command {
    AM2320_command_id id;
    AM2320_register reg;
    int size;
  };

  inline int pack(std::span<std::byte> buffer, const AM2320_command &v) {
    if (std::ssize(buffer) > 0) {
      buffer[0] = static_cast<std::byte>(v.id);
      buffer[1] = static_cast<std::byte>(v.reg);
      buffer[2] = static_cast<std::byte>(v.size);
    }
    return sizeof v;
  }

  class AM2320 {
   public:
    static constexpr auto DeviceAddress = 0x5c;

    static uint16_t crc16(std::span<std::byte> buffer) {
      uint16_t crc = 0xffff;
      for (int i = 0; i < nbytes; i++) {
        uint8_t b = buffer[i];
        crc ^= b;
        for (int x = 0; x < 8; x++) {
          if (crc & 0x0001) {
            crc >>= 1;
            crc ^= 0xA001;
          } else {
            crc >>= 1;
          }
        }
      }
      return crc;
    }

    static uint16_t read(uint8_t register_offset) {
      // wake up
      const auto wake_up = uint8_t{0};
      HAL_I2C_Master_Transmit(hi2c2, DeviceAddress, wake_up, sizeof wake_up,
                              100);
      HAL_Delay(10); // wait 10 ms

      // send a command to read register
      auto command = AM2320_command{AM2320_command_id::ReadRegister,
                                    AM2320_register::Humidity, 2};
      HAL_I2C_Master_Transmit(hi2c2, DeviceAddress, command, sizeof command,
                              100);

      HAL_Delay(2); // wait 2 ms

      // 2 bytes preamble, 2 bytes data, 2 bytes CRC
      uint8_t buffer[6];
      HAL_I2C_Master_Receive(hi2c2, DeviceAddress, buffer, sizeof buffer, 100);

      if (buffer[0] != 0x03)
        return 0xFFFF; // must be 0x03 modbus reply
      if (buffer[1] != 2)
        return 0xFFFF; // must be 2 bytes reply

      const auto received_checksum = (static_cast<uint16_t>(buffer[5]) << 8U)
                                     | buffer[4];
      static_assert(std::is_same_v<decltype(received_checksum), uint16_t>);

      const auto calculated_checksum = crc16(buffer, 4); // preamble + data
      if (received_checksum != calculated_checksum)
        return 0xFFFF;

      // All good!
      const auto register_value = (static_cast<uint16_t>(buffer[2]) << 8U)
                                  | buffer[3];

      return register_value;
    }
  };

} // namespace belmoor

#endif // BELMOOR_AM2320_HPP_
