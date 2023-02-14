#include "nvm.hpp"

#include <catch2/catch.hpp>

namespace belmoor {

  constexpr bool operator==(const M90E26_measurement_registers &lhs,
                            const M90E26_measurement_registers &rhs) {
    return (lhs.Ugain == rhs.Ugain) and (lhs.IgainL == rhs.IgainL)
           and (lhs.IgainN == rhs.IgainN) and (lhs.Uoffset == rhs.Uoffset)
           and (lhs.IoffsetL == rhs.IoffsetL) and (lhs.IoffsetN == rhs.IoffsetN)
           and (lhs.PoffsetL == rhs.PoffsetL) and (lhs.QoffsetL == rhs.QoffsetL)
           and (lhs.PoffsetN == rhs.PoffsetN)
           and (lhs.QoffsetN == rhs.QoffsetN);
  }

  constexpr bool operator==(const Persistent_data &lhs,
                            const Persistent_data &rhs) {
    return (lhs.device.word == rhs.device.word)
           and (lhs.u1_settings == rhs.u1_settings);
  }

  std::ostream &operator<<(std::ostream &os, const Persistent_data &rhs) {
    return os << std::hex << "device: " << rhs.device.word
              << ", u1_settings: [ Ugain: " << rhs.u1_settings.Ugain << ", "
              << "IgainL: " << rhs.u1_settings.IgainL;
  }

  SCENARIO("store and restore", "[NVM]") {
    GIVEN("") {
      const auto original_data = Persistent_data{
          {{false}},
          {0x0123, 0x4567, 0x89ab, 0xcdef, 0x1234, 0x5678, 0x9abc, 0xdef0,
           0x2345, 0x6789}};
      WHEN("storing") {
        store(original_data);
        WHEN("restoring") {
          const auto restored_data = restore();
          REQUIRE(restored_data);
          REQUIRE(*restored_data == original_data);
        }
      }
    }
  }

} // namespace belmoor
