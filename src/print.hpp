#ifndef BELMOOR_PRINT_HPP_
#define BELMOOR_PRINT_HPP_ 1

#include "utils.hpp"

#include <array>
#include <cstdint>
#include <cstdio>
#include <optional>

namespace belmoor {

  inline int print(char *const buffer, const int buffer_len,
                   char const *string) {
    return std::snprintf(buffer, static_cast<size_t>(buffer_len), "%s", string);
  }

  inline int print(char *const buffer, const int buffer_len, const intmax_t v) {
    return std::snprintf(buffer, static_cast<size_t>(buffer_len), "%d",
                         static_cast<int>(v));
  }

  template <typename Tp_>
  inline int print(char *const buffer, const int buffer_len,
                   const std::optional<Tp_> &v) {
    if (v) {
      return print(buffer, buffer_len, *v);
    } else {
      return print(buffer, buffer_len, "--");
    }
  }

  template <size_t Size_>
  inline int print(char *const buffer, const int buffer_len,
                   std::array<char, Size_> const &string) {
    return std::snprintf(buffer, std::min(buffer_len, int{Size_}), "%s",
                         string.data());
  }

  template <size_t Size_, typename Tp_>
  inline int print(std::array<char, Size_> &buffer, Tp_ const &v) {
    return print(buffer.data(), std::ssize(buffer), v);
  }

  template <typename Arg, typename... Args>
  inline int print([[maybe_unused]] char *const buffer,
                   [[maybe_unused]] const int buffer_len, const Arg &head,
                   const Args &...tail) {
    if constexpr (sizeof...(Args) == 0) {
      return 0;
    } else {
      auto const used_characters = print(buffer, buffer_len, head);

      if ((used_characters < 0) or (used_characters >= buffer_len)) {
        return -1;
      } else {
        return print(buffer + used_characters, buffer_len - used_characters,
                     tail...)
               + used_characters;
      }
    }
  }

  template <size_t Size_, typename... Args>
  inline int print(std::array<char, Size_> &buffer, Args const &...args) {
    return print(buffer.data(), std::ssize(buffer), args...);
  }

} // namespace belmoor

#endif // BELMOOR_PRINT_HPP_
