#ifndef BELMOOR_RELAY_HPP_
#define BELMOOR_RELAY_HPP_ 1

#include "sys_tick.hpp"

#include <main.h>
#include <stm32f0xx_hal.h>

namespace belmoor {

  class Button {
    bool previous_{false};

   public:
    [[nodiscard]] static bool pressed() {
      return HAL_GPIO_ReadPin(SW1_nPRESSED_GPIO_Port, SW1_nPRESSED_Pin)
             == GPIO_PIN_RESET;
    }

    [[nodiscard]] bool down() {
      const auto is_pressed = pressed();
      const auto result = is_pressed and (not previous_);
      previous_ = is_pressed;
      return result;
    }

    [[nodiscard]] bool up() {
      const auto is_pressed = pressed();
      const auto result = (not is_pressed) and previous_;
      previous_ = is_pressed;
      return result;
    }
  };

  // The time after which the relay is forced to switch, if no zero crossing
  // was detected.
  constexpr auto Relay_Timeout = Duration{100}; // ms

  class Relay {
    // Whether the relay will be forced to switch after a given time, if no
    // zero crossing is being detected.
    bool should_close_{false};
    bool is_closed_;
    const bool enable_timeout_{false};
    Duration timeout_{Relay_Timeout};

   public:
    constexpr Relay(const bool is_normally_open,
                    const bool enable_timeout) noexcept
        : is_closed_{not is_normally_open}, enable_timeout_{enable_timeout} {}

    [[nodiscard]] bool closed() const { return is_closed_; }
    [[nodiscard]] bool pending() const { return should_close_ != is_closed_; }

    void close() {
      should_close_ = true;
      timeout_ = Relay_Timeout;
    }

    void toggle() {
      should_close_ = not should_close_;
      timeout_ = Relay_Timeout;
    }

    void force() {
      if (should_close_) {
        HAL_GPIO_WritePin(RL1_nENABLE_GPIO_Port, RL1_nENABLE_Pin,
                          GPIO_PIN_RESET);
        is_closed_ = true;
      } else {
        HAL_GPIO_WritePin(RL1_nENABLE_GPIO_Port, RL1_nENABLE_Pin, GPIO_PIN_SET);
        is_closed_ = false;
      }
    }

    void operator()([[maybe_unused]] const Duration period_ms) {
      if (pending() and enable_timeout_) {
        timeout_ -= period_ms;

        if (timeout_ <= 0) {
          force();
        }
      }
    }
  };

} // namespace belmoor

#endif // BELMOOR_RELAY_HPP_
