#ifndef BELMOOR_NVM_HPP_
#define BELMOOR_NVM_HPP_ 1

#include <cstdint>
#include <optional>

namespace belmoor {

  struct Persistent_data {
    static constexpr auto Version = uint32_t{20210521};

    uint16_t Ugain, IgainL, IoffsetL;

    constexpr Persistent_data() = default;

    constexpr Persistent_data(const uint16_t ugain, const uint16_t igainl,
             const uint16_t ioffsetl)
        : Ugain{ugain}, IgainL{igainl}, IoffsetL{ioffsetl} {}

    Persistent_data(const volatile Persistent_data &other)
        : Ugain{other.Ugain}, IgainL{other.IgainL}, IoffsetL{other.IoffsetL} {}
  };

  std::optional<Persistent_data> restore();
  void store(const Persistent_data &data);

} // namespace belmoor

#endif // BELMOOR_NVM_HPP_
