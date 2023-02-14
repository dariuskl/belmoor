#include "m90e26.hpp"

#include "print.hpp"

#include <catch2/catch.hpp>

namespace belmoor {

  template <int ScalingFactor_>
  std::ostream &operator<<(std::ostream &os,
                           const Fixed_point<ScalingFactor_> &rhs) {
    auto buffer = std::array<char, 64>{};
    print(buffer, rhs);
    return os << buffer.data();
  }

  SCENARIO("checksum calculation", "[m90e26]") {
    GIVEN("") {
      const auto vectors = GENERATE(
          std::make_pair<uint16_t, M90E26_measurement_registers>(
              0x2b0bU, {0x6763U, 0x7b19U, 0x7530U, 0U, 8U}),
          std::make_pair<uint16_t, M90E26_measurement_registers>(
              0xdb1bU, {0x679bU, 0x492bU, 0x7530U, 0U, 0U}),
          std::make_pair<uint16_t, M90E26_measurement_registers>(
              0xe000U, {0x6799U, 0x4912U, 0x7530U, 0U, 0U}));
      REQUIRE(vectors.second.cs2() == vectors.first);
    }
  }

  SCENARIO("read power factor", "[m90e26]") {
    GIVEN("") {
      const auto vectors = GENERATE(std::make_pair(0x03e8U, 1.0),
                                    std::make_pair(0x83e8U, -1.0),
                                    std::make_pair(0x0000U, 0.0));
      WHEN("") {
        const auto value = read_power_factor(
            [&](M90E26_register_transfer &msg) {
              msg.rx_data()[1] = static_cast<std::byte>((vectors.first >> 8U)
                                                        & 0xffU);
              msg.rx_data()[2] = static_cast<std::byte>(vectors.first & 0xffU);
              return true;
            });
        THEN("") {
          REQUIRE(value);
          REQUIRE(*value == vectors.second);
        }
      }
    }
  }

  SCENARIO("read real power", "[m90e26]") {
    GIVEN("") {
      const auto vectors = GENERATE(std::make_pair(0x7fffU, 32767.0),
                                    std::make_pair(0xffffU, -32767.0),
                                    std::make_pair(0x0000U, 0.0));
      WHEN("") {
        const auto value = read_real_power(
            [&](M90E26_register_transfer &msg) {
              msg.rx_data()[1] = static_cast<std::byte>((vectors.first >> 8U)
                                                        & 0xffU);
              msg.rx_data()[2] = static_cast<std::byte>(vectors.first & 0xffU);
              return true;
            });
        THEN("") {
          REQUIRE(value);
          REQUIRE(*value == vectors.second);
        }
      }
    }
  }

} // namespace belmoor
