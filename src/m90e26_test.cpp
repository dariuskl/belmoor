#include "m90e26.hpp"

#include <catch2/catch.hpp>

namespace belmoor {

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

} // namespace belmoor
