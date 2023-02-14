#ifndef BELMOOR_SPI1_HPP_
#define BELMOOR_SPI1_HPP_ 1

#include "utils.hpp"

#include <main.h>

#if defined(HAL_SPI_MODULE_ENABLED)
#include <spi.h>
#endif

#if defined(HAL_TIM_MODULE_ENABLED)
#include <tim.h>
#endif

#include <cstddef>
#include <span>

namespace belmoor {

  enum class SPI_state { Idle, Select, Data_Setup, Data_Hold, Deselect };

  // TODO implement queue mode where a list of transfers is cyclically processed
  class Soft_SPI {
    volatile SPI_state state_{SPI_state::Idle};
    const std::byte *volatile tx_{nullptr};
    volatile int tx_size_{0};
    std::byte *volatile rx_{nullptr};
    volatile int rx_size_{0};
    int byte_index_{0};
    int bit_index_{0}; // 7 .. 0

   public:
    void tick() {
      switch (state_) {
      case SPI_state::Idle:
        HAL_GPIO_WritePin(U1_nCS_GPIO_Port, U1_nCS_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(U1_SCK_GPIO_Port, U1_SCK_Pin, GPIO_PIN_SET);
        break;

      case SPI_state::Select:
        HAL_GPIO_WritePin(U1_nCS_GPIO_Port, U1_nCS_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(U1_SCK_GPIO_Port, U1_SCK_Pin, GPIO_PIN_SET);
        state_ = SPI_state::Data_Setup;
        break;

      case SPI_state::Data_Setup:
        HAL_GPIO_WritePin(U1_SCK_GPIO_Port, U1_SCK_Pin, GPIO_PIN_RESET);

        if (byte_index_ < tx_size_) {
          HAL_GPIO_WritePin(U1_MOSI_GPIO_Port, U1_MOSI_Pin,
                            ((tx_[byte_index_] >> bit_index_) & std::byte{1})
                                    != std::byte{0}
                                ? GPIO_PIN_SET
                                : GPIO_PIN_RESET);
        }

        state_ = SPI_state::Data_Hold;
        break;

      case SPI_state::Data_Hold:
        HAL_GPIO_WritePin(U1_SCK_GPIO_Port, U1_SCK_Pin, GPIO_PIN_SET);

        if (byte_index_ < rx_size_) {
          rx_[byte_index_] = rx_[byte_index_]
                             | (HAL_GPIO_ReadPin(U1_MISO_GPIO_Port, U1_MISO_Pin)
                                        == GPIO_PIN_SET
                                    ? std::byte{1}
                                    : std::byte{0})
                                   << bit_index_;
        }

        if (bit_index_ > 0) {
          --bit_index_;
        } else {
          bit_index_ = 7;
          ++byte_index_;
        }

        if ((byte_index_ < tx_size_) or (byte_index_ < rx_size_)) {
          state_ = SPI_state::Data_Setup;
        } else {
          state_ = SPI_state::Deselect;
        }
        break;

      default:
      case SPI_state::Deselect:
        HAL_GPIO_WritePin(U1_SCK_GPIO_Port, U1_SCK_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(U1_nCS_GPIO_Port, U1_nCS_Pin, GPIO_PIN_SET);
        state_ = SPI_state::Idle;
        break;
      }
    }

    [[nodiscard]] SPI_state state() const { return state_; }

    bool read(const std::span<const std::byte> tx_bytes,
              const std::span<std::byte> rx_bytes) {
      if (state_ == SPI_state::Idle) {
        tx_ = tx_bytes.data();
        tx_size_ = static_cast<int>(tx_bytes.size());
        rx_ = rx_bytes.data();
        rx_size_ = static_cast<int>(rx_bytes.size());
        byte_index_ = 0;
        bit_index_ = 7;
        state_ = SPI_state::Select;
        return true;
      } else {
        return false;
      }
    }

    bool write(const std::span<const std::byte> tx_bytes) {
      if (state_ == SPI_state::Idle) {
        tx_ = tx_bytes.data();
        tx_size_ = static_cast<int>(tx_bytes.size());
        rx_size_ = 0;
        byte_index_ = 0;
        bit_index_ = 7;
        state_ = SPI_state::Select;
        return true;
      } else {
        return false;
      }
    }

    bool read_blocking(const std::span<const std::byte> tx_bytes,
                       const std::span<std::byte> rx_bytes) {
      if (read(tx_bytes, rx_bytes)) {
        while (state_ != SPI_state::Idle) {
        }
        return true;
      } else {
        return false;
      }
    }

    bool write_blocking(const std::span<const std::byte> tx_bytes) {
      if (write(tx_bytes)) {
        while (state_ != SPI_state::Idle) {
        }
        return true;
      } else {
        return false;
      }
    }
  };

  template <typename IfType_, typename MessageType_>
  auto spi_read(IfType_ &spi, MessageType_ &msg) {
    return spi.read_blocking(msg.tx_data(), msg.rx_data());
  }

  template <typename IfType_, typename MessageType_>
  auto spi_write(IfType_ &spi, MessageType_ &msg) {
    return spi.write_blocking(msg.tx_data());
  }

#if HAL_SPI_MODULE_ENABLED

  template <typename MessageType_>
  auto spi_read(SPI_HandleTypeDef &spi, MessageType_ &msg,
                const uint32_t timeout) {
    return HAL_SPI_TransmitReceive(&spi, msg.tx, &(msg.rx.unused_),
                                   sizeof msg.tx, timeout)
           == HAL_OK;
  }

  template <typename MessageType_>
  auto spi_write(SPI_HandleTypeDef &spi, MessageType_ &msg,
                 const uint32_t timeout) {
    return HAL_SPI_Transmit(&spi, msg.tx, sizeof msg.tx, timeout) == HAL_OK;
  }

#endif

  class SPI_read {
    Soft_SPI &spi_;

   public:
    explicit constexpr SPI_read(Soft_SPI &spi) : spi_{spi} {}

    template <typename MessageType_> auto operator()(MessageType_ &msg) {
      return spi_read(spi_, msg);
    }
  };

  class SPI_write {
    Soft_SPI &spi_;

   public:
    explicit constexpr SPI_write(Soft_SPI &spi) : spi_{spi} {}

    template <typename MessageType_> auto operator()(MessageType_ &msg) {
      return spi_write(spi_, msg);
    }
  };

} // namespace belmoor

#endif // BELMOOR_SPI1_HPP_
