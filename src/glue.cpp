// The code in this file "glues" the STM HAL and the generated initialization
// code to the application firmware.

#include "app.hpp"

#include <main.h>

#include <cstdint>

namespace belmoor {

  std::function<void()> on_u1_zx_edge;
  std::function<void()> on_sw1_falling;

} // namespace belmoor

extern "C" void mainloop() { belmoor::mainloop(); }

extern "C" void HAL_GPIO_EXTI_Callback(const uint16_t GPIO_Pin) {
  switch (GPIO_Pin) {
  case U1_ZX_Pin:
    if (belmoor::on_u1_zx_edge) {
      belmoor::on_u1_zx_edge();
    }
    break;

  case SW1_nPRESSED_Pin:
    if (belmoor::on_sw1_falling) {
      belmoor::on_sw1_falling();
    }
    break;

  default:
    break;
  }
}
