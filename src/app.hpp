#ifndef BELMOOR_APP_HPP_
#define BELMOOR_APP_HPP_ 1

#include <functional>

namespace belmoor {

  extern std::function<void()> on_u1_zx_edge;
  extern std::function<void()> on_sw1_falling;

  [[noreturn]] void mainloop();

} // namespace belmoor

#endif // BELMOOR_APP_HPP_
