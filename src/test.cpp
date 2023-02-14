#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <array>
#include <cstddef>
#include <queue>

auto GPIO = std::array<uint32_t, 5>{};
auto usb_transmit_buffer = std::queue<std::byte>{};
