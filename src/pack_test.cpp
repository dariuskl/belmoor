#include "pack.hpp"

#include <catch2/catch.hpp>

namespace belmoor {

  SCENARIO("basics", "[pack]") {
    auto buffer = std::array<std::byte, 32>{};

    WHEN("packing an i8") {
      CHECK(pack(buffer, i8{-1}) == 1);
      CHECK(buffer.at(0) == std::byte{0xff});
    }

    WHEN("packing a u8") {
      CHECK(pack(buffer, u8{0x34}) == 1);
      CHECK(buffer.at(0) == std::byte{0x34});
    }

    WHEN("packing a u16") {
      CHECK(pack(buffer, u16{0x5678}) == 2);
      CHECK(buffer.at(0) == std::byte{0x78});
      CHECK(buffer.at(1) == std::byte{0x56});
    }

    WHEN("packing multiple variables") {
      CHECK(pack(buffer, i8{0}, u8{0x34}, u16{0x5678}) == 4);
      CHECK(buffer.at(0) == std::byte{0});
      CHECK(buffer.at(1) == std::byte{0x34});
      CHECK(buffer.at(2) == std::byte{0x78});
      CHECK(buffer.at(3) == std::byte{0x56});
    }
  }

} // namespace belmoor
