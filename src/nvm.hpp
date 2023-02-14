#ifndef BELMOOR_NVM_HPP_
#define BELMOOR_NVM_HPP_ 1

#include <cstdint>
#include <optional>

namespace belmoor {

  struct NVM {
    uint16_t Ugain, IgainL, IgainN, Uoffset, IoffsetL, IoffsetN;
  };

  std::optional<NVM> restore();
  void store(const NVM& data);

} // namespace belmoor

#endif // BELMOOR_NVM_HPP_
