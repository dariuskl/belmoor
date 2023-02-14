
#ifndef BELMOOR_M90E26_HPP_
#define BELMOOR_M90E26_HPP_ 1

#include "utils.hpp"

#include <cstddef>
#include <cstdint>
#include <numeric>
#include <optional>
#include <span>

namespace belmoor {

  enum class M90E26_register : uint8_t {
    SoftReset,
    SysStatus,
    LastData = 0x06U, // Last data that was read/written via SPI
    MMode = 0x2bU,
    CS1 = 0x2cU,
    AdjStart = 0x30U, // Measurement Calibration Start Command
    U_gain = 0x31U,
    I_gain_L = 0x32U,
    I_offset_L = 0x35U,
    CS2 = 0x3bU,
    I_rms = 0x48U,  // RMS current trough Live
    U_rms = 0x49U,  // RMS voltage between Live and Neutral
    P_mean = 0x4aU, // mean active power over Live
    Q_mean = 0x4bU, // mean reactive power over Live
    Freq = 0x4cU,   // voltage frequency, 0.01 Hz
    PowerF = 0x4dU, // power factor of Live, 0.001
    S_mean = 0x4fU, // mean apparent power over Live
  };

  constexpr auto M90E26_SysStatus_SagWarn = uint16_t{0x0002};
  constexpr auto M90E26_SysStatus_RevPchg = uint16_t{0x0020};
  constexpr auto M90E26_SysStatus_RevQchg = uint16_t{0x0040};
  constexpr auto M90E26_SysStatus_LNchange = uint16_t{0x0080};
  constexpr auto M90E26_SysStatus_AdjErr = uint16_t{0x7000};
  constexpr auto M90E26_SysStatus_CalErr = uint16_t{0xC000};

  constexpr auto M90E26_MMode_Lgain_Mask = uint16_t{0xe000};
  constexpr auto M90E26_MMode_Lgain_1 = uint16_t{0x8000};
  constexpr auto M90E26_MMode_Lgain_4 = uint16_t{0x0000};
  constexpr auto M90E26_MMode_Lgain_8 = uint16_t{0x2000};
  constexpr auto M90E26_MMode_Lgain_16 = uint16_t{0x4000};
  constexpr auto M90E26_MMode_Lgain_24 = uint16_t{0x6000};

  constexpr auto M90E26_AdjStart_NoMeasurement = 0x6886U;
  constexpr auto M90E26_AdjStart_Adjustment = 0x5678U;
  constexpr auto M90E26_AdjStart_Measurement = 0x8765U;

  class M90E26_register_transfer {
    std::byte tx_[3];
    std::byte rx_[3];

   public:
    constexpr explicit M90E26_register_transfer(const M90E26_register address)
        : tx_{std::byte{
                  static_cast<uint8_t>(0x80U | static_cast<unsigned>(address))},
              std::byte{0U}, std::byte{0U}},
          rx_{} {}

    constexpr M90E26_register_transfer(const M90E26_register address,
                                       const uint16_t value)
        : tx_{std::byte{
                  static_cast<uint8_t>(0x7FU & static_cast<unsigned>(address))},
              std::byte{static_cast<uint8_t>(value >> 8U)},
              std::byte{static_cast<uint8_t>(value)}},
          rx_{} {}

    [[nodiscard]] std::span<const std::byte> tx_data() const {
      return std::span(tx_);
    }

    [[nodiscard]] std::span<std::byte> rx_data() { return std::span(rx_); }

    [[nodiscard]] uint16_t rx_value() const {
      return static_cast<uint16_t>((static_cast<uint16_t>(rx_[1]) << 8U)
                                   | static_cast<uint16_t>(rx_[2]));
    }
  };

  template <class WriteFn_> bool reset(WriteFn_ write) {
    if (auto msg = M90E26_register_transfer{M90E26_register::SoftReset, 0x789};
        write(msg)) {
      return true;
    } else {
      return false;
    }
  }

  template <class ReadFn_> std::optional<uint16_t> read_status(ReadFn_ read) {
    if (auto msg = M90E26_register_transfer{M90E26_register::SysStatus};
        read(msg)) {
      return {msg.rx_value()};
    } else {
      return {std::nullopt};
    }
  }

  template <class ReadFn_>
  std::optional<Fixed_point<1000>> read_current(ReadFn_ read) {
    if (auto msg = M90E26_register_transfer{M90E26_register::I_rms};
        read(msg)) {
      return {{msg.rx_value()}};
    } else {
      return {};
    }
  }

  template <class ReadFn_>
  std::optional<Fixed_point<100>> read_voltage(ReadFn_ read) {
    if (auto msg = M90E26_register_transfer{M90E26_register::U_rms};
        read(msg)) {
      return {{msg.rx_value()}};
    } else {
      return {};
    }
  }

  template <class ReadFn_>
  std::optional<Fixed_point<1000>> read_real_power(ReadFn_ read) {
    if (auto msg = M90E26_register_transfer{M90E26_register::P_mean};
        read(msg)) {
      return {{static_cast<int16_t>(msg.rx_value())}};
    } else {
      return {};
    }
  }

  template <class ReadFn_>
  std::optional<Fixed_point<1000>> read_reactive_power(ReadFn_ read) {
    if (auto msg = M90E26_register_transfer{M90E26_register::Q_mean};
        read(msg)) {
      return {{static_cast<int16_t>(msg.rx_value())}};
    } else {
      return {};
    }
  }

  template <class ReadFn_>
  std::optional<Fixed_point<100>> read_frequency(ReadFn_ read) {
    if (auto msg = M90E26_register_transfer{M90E26_register::Freq}; read(msg)) {
      return {{msg.rx_value()}};
    } else {
      return {};
    }
  }

  template <class ReadFn_>
  std::optional<Fixed_point<1000>> read_power_factor(ReadFn_ read) {
    if (auto msg = M90E26_register_transfer{M90E26_register::PowerF};
        read(msg)) {
      const auto data = msg.rx_value();
      return {{static_cast<int32_t>(data & ~0x8000)
               * static_cast<int32_t>((data & 0x8000) ? -1 : 1)}};
    } else {
      return {};
    }
  }

  template <class ReadFn_>
  std::optional<Fixed_point<1000>> read_apparent_power(ReadFn_ read) {
    if (auto msg = M90E26_register_transfer{M90E26_register::S_mean};
        read(msg)) {
      return {{static_cast<int16_t>(msg.rx_value())}};
    } else {
      return {};
    }
  }

  struct [[gnu::packed]] M90E26_metering_registers {
    uint16_t PLconstH{0x0015U}, PLconstL{0xd174U}, Lgain{0U}, Lphi{0U},
        Ngain{0U}, Nphi{0U}, PStartTh{0x08bdU}, PNolTh{0U}, QStartTh{0x0aecU},
        QNolTh{0U}, MMode{0x9422U};

    [[nodiscard]] uint16_t cs1() const {
      const auto bytes = std::as_bytes(std::span{this, 1});
      const auto lh2c = std::accumulate(
          bytes.begin(), bytes.end(), std::pair<uint8_t, uint8_t>{0U, 0U},
          [](const std::pair<uint8_t, uint8_t> &sum,
             const std::byte &rhs) -> std::pair<uint8_t, uint8_t> {
            return {sum.first + static_cast<uint8_t>(rhs),
                    sum.second ^ static_cast<uint8_t>(rhs)};
          });
      return static_cast<uint16_t>((static_cast<uint16_t>(lh2c.second) << 8U)
                                   | lh2c.first);
    }
  };

  struct [[gnu::packed]] M90E26_measurement_registers {
    uint16_t Ugain{0x6720U}, IgainL{0x7A13U}, IgainN{0x7530U}, Uoffset{0U},
        IoffsetL{0U}, IoffsetN{0U}, PoffsetL{0U}, QoffsetL{0U}, PoffsetN{0U},
        QoffsetN{0U};

    [[nodiscard]] uint16_t cs2() const {
      const auto bytes = std::as_bytes(std::span{this, 1});
      const auto lh3b = std::accumulate(
          bytes.begin(), bytes.end(), std::pair<uint8_t, uint8_t>{0U, 0U},
          [](const std::pair<uint8_t, uint8_t> &sum,
             const std::byte &rhs) -> std::pair<uint8_t, uint8_t> {
            return {sum.first + static_cast<uint8_t>(rhs),
                    sum.second ^ static_cast<uint8_t>(rhs)};
          });
      return static_cast<uint16_t>((static_cast<uint16_t>(lh3b.second) << 8U)
                                   | lh3b.first);
    }
  };

} // namespace belmoor

#endif // BELMOOR_M90E26_HPP_
