#ifndef BELMOOR_UTILS_HPP_
#define BELMOOR_UTILS_HPP_ 1

#include <array>
#include <cinttypes>
#include <cmath>
#include <cstdio>

namespace belmoor {

  struct i8 {
    int8_t v;
  };

  struct u8 {
    uint8_t v;
  };

  inline bool operator!=(const u8 lhs, const u8 rhs) { return lhs.v != rhs.v; }

  inline u8 operator<<(const u8 lhs, const int rhs) {
    return {static_cast<uint8_t>(lhs.v << rhs)};
  }

  inline u8 operator>>(const u8 lhs, const int rhs) {
    return {static_cast<uint8_t>(lhs.v >> rhs)};
  }

  inline u8 operator&(const u8 lhs, const u8 rhs) {
    return {static_cast<uint8_t>(lhs.v & rhs.v)};
  }

  inline u8 operator|(const u8 lhs, const u8 rhs) {
    return {static_cast<uint8_t>(lhs.v | rhs.v)};
  }

  struct u16 {
    uint16_t v;
  };

  inline bool operator==(const u16 lhs, const u16 rhs) {
    return lhs.v == rhs.v;
  }

  inline bool operator!=(const u16 lhs, const u16 rhs) {
    return lhs.v != rhs.v;
  }

  inline u16 operator>>(const u16 lhs, const int rhs) {
    return {static_cast<uint16_t>(lhs.v >> rhs)};
  }

  inline u8 operator&(const u16 lhs, const u8 rhs) {
    return {static_cast<uint8_t>(lhs.v & rhs.v)};
  }

  inline u16 operator&(const u16 lhs, const u16 rhs) {
    return {static_cast<uint16_t>(lhs.v & rhs.v)};
  }

  inline u16 operator|(const u16 lhs, const u16 rhs) {
    return {static_cast<uint16_t>(lhs.v | rhs.v)};
  }

  struct u32 {
    uint32_t v;
  };

  inline bool operator==(const u32 lhs, const u32 rhs) {
    return lhs.v == rhs.v;
  }

  inline u32 operator>>(const u32 lhs, const int rhs) { return {lhs.v >> rhs}; }

  inline u16 bswap(const u16 u) { return {__builtin_bswap16(u.v)}; }
  inline u32 bswap(const u32 u) { return {__builtin_bswap32(u.v)}; }

  namespace {

    constexpr auto integral(const int32_t v, const int scaling_factor) {
      return v / scaling_factor;
    }

    constexpr auto fractional(const int32_t v, const int scaling_factor) {
      return v % scaling_factor;
    }

  } // namespace

  template <int ScalingFactor_> struct Fixed_point { int32_t raw; };

  template <int ScalingFactor_>
  constexpr bool operator>(const Fixed_point<ScalingFactor_> &lhs,
                           const double rhs) {
    return lhs.raw > static_cast<decltype(lhs.raw)>(rhs * ScalingFactor_);
  }

  template <int ScalingFactor_>
  int print(char *const buffer, const int buffer_len,
            Fixed_point<ScalingFactor_> fp) {
    static_assert((ScalingFactor_ == 1) or (ScalingFactor_ == 10)
                  or (ScalingFactor_ == 100) or ScalingFactor_ == 1000);

    if (buffer_len < 0) {
      return -1;
    }

    auto &v = fp.raw;

    const auto buffer_size = static_cast<size_t>(buffer_len);

    if (ScalingFactor_ == 1) {
      return std::snprintf(buffer, buffer_size, "%" PRId32, v);
    } else {
      const auto intgl = integral(v, ScalingFactor_);
      const auto fract = fractional(v, ScalingFactor_);

      const auto zeros = ScalingFactor_ == 10     ? 1
                         : ScalingFactor_ == 100  ? 2
                         : ScalingFactor_ == 1000 ? 3
                                                  : 0;
      const auto sign = fract < 0 ? "-" : "";

      static auto fmt = std::array<char, 32>{};

      std::snprintf(fmt.data(), fmt.size(), "%s%%" PRId32 ".%%0%d" PRId32, sign,
                    zeros);

      return std::snprintf(buffer, buffer_size, fmt.data(), std::abs(intgl),
                           std::abs(fract));
    }
  }

} // namespace belmoor

#endif // BELMOOR_UTILS_HPP_
