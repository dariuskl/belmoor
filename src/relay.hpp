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

  class Relay {
    bool is_normally_open_{false};
    bool should_close_{false};
    bool is_closed_;

   public:
    constexpr explicit Relay(const bool is_normally_open) noexcept
        : is_normally_open_{is_normally_open}, is_closed_{
                                                   not is_normally_open} {}

    [[nodiscard]] bool closed() const { return is_closed_; }
    [[nodiscard]] bool pending() const { return should_close_ != is_closed_; }

    void close() { should_close_ = true; }
    void toggle() { should_close_ = not should_close_; }

    void update() {
      if (should_close_) {
        HAL_GPIO_WritePin(RL1_nENABLE_GPIO_Port, RL1_nENABLE_Pin,
                          is_normally_open_ ? GPIO_PIN_SET : GPIO_PIN_RESET);
        is_closed_ = true;
      } else {
        HAL_GPIO_WritePin(RL1_nENABLE_GPIO_Port, RL1_nENABLE_Pin,
                          is_normally_open_ ? GPIO_PIN_RESET : GPIO_PIN_SET);
        is_closed_ = false;
      }
    }
  };

} // namespace belmoor

#endif // BELMOOR_RELAY_HPP_
