
#ifndef SRC_FAKE_STM32F0XX_HAL_FLASH_H
#define SRC_FAKE_STM32F0XX_HAL_FLASH_H

#include "stm32f0xx_hal.h"

typedef struct {
  uint32_t TypeErase;
  intptr_t PageAddress;
  uint32_t NbPages;
} FLASH_EraseInitTypeDef;

constexpr auto FLASH_TYPEERASE_PAGES = 0U;
constexpr auto FLASH_TYPEPROGRAM_WORD = 0U;

inline auto HAL_FLASH_Unlock() {
  FLASH->SR &= ~FLASH_CR_LOCK;
  return HAL_OK;
}

inline void HAL_FLASH_Lock() { FLASH->SR |= FLASH_CR_LOCK; }

inline auto
HAL_FLASHEx_Erase([[maybe_unused]] FLASH_EraseInitTypeDef *const config,
                  uint32_t *const page_error) {
  // FIXME: set storage_ to 0xff bytes
  *page_error = 0xffffffffU;
  return HAL_OK;
}

inline auto HAL_FLASH_Program(const int program_mode, const uintptr_t address,
                              const uint64_t value) {
  switch (program_mode) {
  case FLASH_TYPEPROGRAM_WORD:
    *reinterpret_cast<uint32_t *>(address) = static_cast<uint32_t>(value);
    return HAL_OK;

  default:
    return ~HAL_OK;
  }
}

#endif // SRC_FAKE_STM32F0XX_HAL_FLASH_H
