#include "nvm.hpp"

#include <stm32f0xx_hal_flash.h>
#include <stm32f0xx_hal_rcc.h>

#include <span>

namespace belmoor {

  namespace {

    template <typename Data_> uint32_t crc32(std::span<Data_> data) {
      __HAL_RCC_CRC_CLK_ENABLE();
      CRC->CR = CRC->CR | CRC_CR_RESET;
      for (const auto &b : std::as_bytes(data)) {
        CRC->DR = static_cast<uint8_t>(b);
      }
      const auto result = CRC->DR;
      __HAL_RCC_CRC_CLK_DISABLE();
      return result;
    }

    union NVM {
      struct {
        uint32_t checksum;
        uint32_t version;
        Persistent_data payload;
      } data;
      struct alignas(FLASH_PAGE_SIZE) {
        std::array<std::byte, FLASH_PAGE_SIZE> bytes{};
      } page;
    };

    const auto storage_ = NVM{};

  } // namespace

  std::optional<Persistent_data> restore() {
    if (*static_cast<const volatile uint32_t *>(&storage_.data.version)
        == Persistent_data::Version) {
      return storage_.data.payload;
    } else {
      return {};
    }
  }

  void store(const Persistent_data &data) {
    const auto checksum = crc32(std::span{&data, 1});
    auto erase_config = FLASH_EraseInitTypeDef{
        FLASH_TYPEERASE_PAGES,
        static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&storage_)), 1};
    auto page_error = uint32_t{0};
    if ((HAL_FLASH_Unlock() == HAL_OK)
        and (HAL_FLASHEx_Erase(&erase_config, &page_error) == HAL_OK)
        and (page_error == 0xffffffffU)) {
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                        reinterpret_cast<uintptr_t>(&(storage_.data.checksum)),
                        checksum);
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                        reinterpret_cast<uintptr_t>(&(storage_.data.version)),
                        Persistent_data::Version);
      const auto *words = reinterpret_cast<const uint32_t *>(&data);
      for (auto i = 0; i < (1 + ((int{sizeof data} - 1) / 4)); ++i) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                          reinterpret_cast<uintptr_t>(&(storage_.data.payload))
                              + static_cast<uint32_t>(i * 4),
                          words[i]);
      }
    }
    HAL_FLASH_Lock();
    /* FIXME the CPU does not see the changed flash memory contents until after
     *    a power cycle?! */
  }

} // namespace belmoor
