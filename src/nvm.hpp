#ifndef BELMOOR_NVM_HPP_
#define BELMOOR_NVM_HPP_ 1

#include "m90e26.hpp"

#include <cstdint>
#include <optional>

namespace belmoor {

  struct Persistent_data {
    static constexpr auto Version = uint32_t{20210624};

    union {
      struct {
        // The Belmoors will have the relays mounted such that they allow power
        // to a connected device, if the coil is not powered. This is considered
        // "normally closed".
        //   If the Belmoor was modified, to have the relay mounted normally
        //   open,
        // this variable should be set `true`.
        uint32_t relay_normally_open : 1;
      } options;
      uint32_t word;
    } device{{false}};

    M90E26_measurement_registers u1_settings;
  };

  std::optional<Persistent_data> restore();
  void store(const Persistent_data &data);

} // namespace belmoor

#endif // BELMOOR_NVM_HPP_
