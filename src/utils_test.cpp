#include "utils.hpp"

#include <catch2/catch.hpp>

namespace belmoor {

  template <typename Tp1_, typename Tp2_>
  std::ostream &operator<<(std::ostream &os, const std::pair<Tp1_, Tp2_> rhs) {
    return os << "(" << rhs.first << ", " << rhs.second << ")";
  }

} // namespace belmoor
