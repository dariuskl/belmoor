#include "m90e26.hpp"

#include <catch2/catch.hpp>

namespace belmoor {

  SCENARIO("checksum calculation", "[m90e26]") {
    GIVEN("") {
      auto uut = M90E26_measurement_registers{0x6763U, 0x7b19U, 0x7530U, 0U, 8U};
      REQUIRE(uut.cs2() == 0x2b0bU);
    }
  }

} // namespace belmoor
