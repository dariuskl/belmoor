
#ifndef BELMOOR_SYS_TICK_HPP_
#define BELMOOR_SYS_TICK_HPP_ 1

#include <stm32f0xx_hal.h>

#include <cstdint>
#include <limits>

namespace belmoor {

  using Duration = intmax_t;

  void sleep_for(const Duration d) {
    HAL_Delay(static_cast<uint32_t>(d));
  }

  class Sys_tick {
    Duration previous_tick_;

   public:
    Sys_tick() : previous_tick_{HAL_GetTick()} {}

    [[nodiscard]] auto tick() const { return previous_tick_; }

    auto operator()() {
      const auto tick = Duration{HAL_GetTick()};
      const auto period = tick - previous_tick_;
      previous_tick_ = tick;
      if (period < 0) {
        return period + std::numeric_limits<decltype(period)>::max();
      } else {
        return period;
      }
    }
  };

} // namespace belmoor

#endif // BELMOOR_SYS_TICK_HPP_
