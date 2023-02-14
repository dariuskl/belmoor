#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_flash.h>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <queue>

std::array<uint32_t, 5> GPIO{};
auto usb_transmit_buffer = std::queue<std::byte>{};

auto CRC_ = CRC_TypeDef{};
CRC_TypeDef *const CRC = &CRC_;

auto FLASH_ = FLASH_TypeDef{};
FLASH_TypeDef *const FLASH = &FLASH_;
