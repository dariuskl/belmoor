#ifndef BELMOOR_PACK_HPP_
#define BELMOOR_PACK_HPP_ 1

#include "utils.hpp"

#include <cstdint>
#include <span>

namespace belmoor {

  inline int pack(std::span<std::byte> buffer, const i8 v) {
    if (std::ssize(buffer) > 0) {
      buffer[0] = std::byte{static_cast<uint8_t>(v.v)};
    }
    return 1;
  }

  inline int pack(std::span<std::byte> buffer, const u8 v) {
    if (std::ssize(buffer) > 0) {
      buffer[0] = std::byte{v.v};
    }
    return 1;
  }

  inline int pack(std::span<std::byte> buffer, const u16 v) {
    if (std::ssize(buffer) > 0) {
      buffer[0] = std::byte{(v & u8{0xff}).v};
      if (std::ssize(buffer) > 1) {
        buffer[1] = std::byte{static_cast<uint8_t>((v >> 8).v)};
      }
    }
    return 2;
  }

  inline int pack([[maybe_unused]] std::span<std::byte> buffer) { return 0; }

  template <typename Arg1_, typename Arg2_, typename... Args_>
  inline int pack(std::span<std::byte> buffer, const Arg1_ &first,
                  const Arg2_ &second, const Args_ &...tail) {
    if (auto const first_bytes = pack(buffer, first);
        (first_bytes < 0) or (first_bytes >= std::ssize(buffer))) {
      return -1;
    } else if (auto const second_bytes = pack(
                   buffer.subspan(static_cast<size_t>(first_bytes)), second);
               (second_bytes < 0) or (second_bytes >= std::ssize(buffer))) {
      return -1;
    } else {
      return pack(buffer.subspan(
                      static_cast<size_t>(first_bytes + second_bytes)),
                  tail...)
             + first_bytes + second_bytes;
    }
  }

} // namespace belmoor

#endif // BELMOOR_PACK_HPP_
