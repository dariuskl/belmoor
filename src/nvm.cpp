#include "nvm.hpp"
#include "utils.hpp"

#include <stm32f0xx_hal_flash.h>

namespace belmoor {

  namespace {
    constexpr auto Version = u32{20210521};

    const auto version_ = u32{0};
    const auto data_ = NVM{};
  } // namespace

  std::optional<NVM> restore() {
    if (version_ == Version) {
      return data_;
    } else {
      return {};
    }
  }

  void store(const NVM &data) {
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                      reinterpret_cast<uint32_t>(&version_), 20210521UL);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,
                      reinterpret_cast<uint32_t>(&(data_.Ugain)), data.Ugain);
  }

} // namespace belmoor
