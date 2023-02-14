#include <main.h>

#include "display.hpp"
#include "m90e26.hpp"
#include "relay.hpp"
#include "spi.hpp"
#include "usb.hpp"
#include "utils.hpp"

namespace belmoor {

  // Because the metering IC is powered independently of the MCU, we have to
  // handle unsolicited resets.
  enum class Operating_mode {
    Offline, // Communication with the metering IC cannot be established. This
             // is normally the case, if there are no mains connected and thus
             // no primary-side power.
    SetUp,   // The metering IC is set up with any stored calibration data.
    Normal,
    Calibration
  };

  constexpr auto operating_mode_texts = std::array<const char *, 4>{
      {"OFF", "INI", "", "CAL"}};

  constexpr auto AdditionalInfo_Period = Duration{1000}; // ms

  auto relay = Relay{true, false};

  // When initially powered, the device will fully turn on and then off all
  // indicators for a given duration in order to provide the user with a means
  // to identify failure of any.
  void prove_out(Display &display) {
    HAL_GPIO_WritePin(LD1_GREEN_GPIO_Port, LD1_GREEN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LD1_RED_GPIO_Port, LD1_RED_Pin, GPIO_PIN_RESET);
    display.fill().update();

    sleep_for(500);

    HAL_GPIO_WritePin(LD1_GREEN_GPIO_Port, LD1_GREEN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LD1_RED_GPIO_Port, LD1_RED_Pin, GPIO_PIN_SET);

    sleep_for(500);

    HAL_GPIO_WritePin(LD1_GREEN_GPIO_Port, LD1_GREEN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LD1_RED_GPIO_Port, LD1_RED_Pin, GPIO_PIN_RESET);
    display.clear().update();

    sleep_for(500);
  }

  Operating_mode next_operating_mode(const Operating_mode current,
                                     std::optional<uint16_t> status,
                                     std::optional<uint16_t> adj_start) {
    switch (current) {
    default:
    case Operating_mode::Offline:
      if (not status) {
        return Operating_mode::Offline;
      } else {
        return Operating_mode::SetUp;
      }

    case Operating_mode::SetUp:
      if ((not status) or (not adj_start)) {
        return Operating_mode::Offline;
      } else if (*adj_start == M90E26_AdjStart_Measurement) {
        return Operating_mode::Normal;
      } else if (*adj_start == M90E26_AdjStart_Adjustment) {
        return Operating_mode::Calibration;
      } else {
        return Operating_mode::SetUp;
      }

    case Operating_mode::Normal:
      if ((not status) or (not adj_start)) {
        return Operating_mode::Offline;
      } else if (*adj_start == M90E26_AdjStart_NoMeasurement) {
        return Operating_mode::SetUp;
      } else if (*adj_start == M90E26_AdjStart_Adjustment) {
        return Operating_mode::Calibration;
      } else {
        return Operating_mode::Normal;
      }

    case Operating_mode::Calibration:
      if ((not status) or (not adj_start)) {
        return Operating_mode::Offline;
      } else if (*adj_start == M90E26_AdjStart_NoMeasurement) {
        return Operating_mode::SetUp;
      } else if (*adj_start == M90E26_AdjStart_Measurement) {
        return Operating_mode::Normal;
      } else {
        return Operating_mode::Calibration;
      }
    }
  }

  [[noreturn]] void mainloop() {
    static auto u1_spi = Soft_SPI{};
    htim2.PeriodElapsedCallback = [](TIM_HandleTypeDef *) { u1_spi.tick(); };

    auto display = Display{};
    auto button = Button{};
    auto status = std::optional<uint16_t>{};
    auto adj_start = std::optional<uint16_t>{};
    auto il_rms = std::optional<Fixed_point<1000>>{};
    auto u_rms = std::optional<Fixed_point<100>>{};
    auto pl_mean = std::optional<Fixed_point<1000>>{};
    auto sl_mean = std::optional<Fixed_point<1000>>{};
    auto ql_mean = std::optional<Fixed_point<1000>>{};
    auto freq = std::optional<Fixed_point<100>>{};
    auto pfl = std::optional<Fixed_point<1000>>{};
    auto additional_info_index = 0;
    auto additional_info_timer = AdditionalInfo_Period;
    auto operating_mode = Operating_mode::Offline;

    relay.close();

    prove_out(display);

    auto sys_tick = Sys_tick{};
    while (true) {
      const auto period = sys_tick();

      if (button.down()) {
        relay.toggle();
      }

      relay(period);

      display.clear();

      if (auto st = read_status(SPI_read{u1_spi}); st) {
        status = st;

        if (auto msg = M90E26_register_transfer{M90E26_register::AdjStart};
            spi_read(u1_spi, msg)) {
          adj_start = msg.rx_value();
        } else {
          adj_start = std::nullopt;
        }

        pfl = read_power_factor(SPI_read{u1_spi});
        freq = read_frequency(SPI_read{u1_spi});
        il_rms = read_current(SPI_read{u1_spi});
        u_rms = read_voltage(SPI_read{u1_spi});
        pl_mean = read_real_power(SPI_read{u1_spi});
        sl_mean = read_apparent_power(SPI_read{u1_spi});
        ql_mean = read_reactive_power(SPI_read{u1_spi});
      } else {
        adj_start = std::nullopt;
        pfl = std::nullopt;
        freq = std::nullopt;
        il_rms = std::nullopt;
        u_rms = std::nullopt;
        pl_mean = std::nullopt;
        sl_mean = std::nullopt;
        ql_mean = std::nullopt;
      }

      operating_mode = next_operating_mode(operating_mode, status, adj_start);

      switch (operating_mode) {
      case Operating_mode::Offline:
        HAL_GPIO_WritePin(LD1_RED_GPIO_Port, LD1_RED_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LD1_GREEN_GPIO_Port, LD1_GREEN_Pin, GPIO_PIN_RESET);
        break;

      case Operating_mode::SetUp:
        HAL_GPIO_WritePin(LD1_RED_GPIO_Port, LD1_RED_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LD1_GREEN_GPIO_Port, LD1_GREEN_Pin, GPIO_PIN_RESET);
        // TODO write persisted calibration values to the IC
        display.set_font(Font_11x18)
            .at(0, 0)
            .print("Needs")
            .at(0, 20)
            .print("Calibration")
            .set_font(Font_7x10)
            .at(0, 40);
        switch (additional_info_index) {
        default:
        case 0:
        case 1:
        case 2:
          display.print("Status: ", u16{*status});
          break;

        case 3:
        case 4:
          display.print("AdjStart: ", u16{*adj_start});
          break;
        }
        break;

      case Operating_mode::Normal:
        if (*status != 0U) {
          HAL_GPIO_WritePin(LD1_RED_GPIO_Port, LD1_RED_Pin, GPIO_PIN_SET);
          display.set_font(Font_11x18)
              .at(0, 0)
              .print("Error")
              .set_font(Font_7x10)
              .at(0, 20)
              .print("status: ", status);
        } else {
          HAL_GPIO_WritePin(LD1_RED_GPIO_Port, LD1_RED_Pin, GPIO_PIN_RESET);
        }
        if (*u_rms > 5.0) {
          HAL_GPIO_WritePin(LD1_GREEN_GPIO_Port, LD1_GREEN_Pin, GPIO_PIN_SET);
        } else {
          HAL_GPIO_WritePin(LD1_GREEN_GPIO_Port, LD1_GREEN_Pin, GPIO_PIN_RESET);
        }
        display.set_font(Font_11x18)
            .at(0, 0)
            .print(u_rms, " V")
            .at(0, 20)
            .print(il_rms, " A")
            .set_font(Font_7x10)
            .at(0, 40);

        switch (additional_info_index) {
        default:
        case 0:
          display.print(pl_mean, " kW");
          break;

        case 1:
          display.print(sl_mean, " kVA");
          break;

        case 2:
          display.print(ql_mean, " kvar");
          break;

        case 3:
          display.print(freq, " Hz");
          break;

        case 4:
          display.print(pfl);
          break;
        }
        break;

      case Operating_mode::Calibration:
        HAL_GPIO_WritePin(LD1_RED_GPIO_Port, LD1_RED_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LD1_GREEN_GPIO_Port, LD1_GREEN_Pin, GPIO_PIN_RESET);

        display.set_font(Font_11x18)
            .at(0, 0)
            .print(u_rms, " V")
            .at(0, 20)
            .print(il_rms, " A");
        break;
      }

      if (additional_info_timer <= 0) {
        if (additional_info_index < 4) {
          ++additional_info_index;
        } else {
          additional_info_index = 0;
        }
        additional_info_timer = AdditionalInfo_Period;
      } else {
        additional_info_timer -= period;
      }

      if (relay.pending()) {
        display.set_font(Font_6x8).at(75, 55).print("PENDING");
      }

      if (relay.closed()) {
        // ______
        display.at(0, 63).line_to(127, 63);
      } else {
        // __/ __
        display.at(0, 63).line_to(47, 63).line_to(63, 55).at(63, 63).line_to(
            127, 63);
      }

      display.at(100, 0)
          .set_font(Font_7x10)
          .print(operating_mode_texts[static_cast<size_t>(operating_mode)])
          .update();

      if (auto message_buffer = usb::Message_buffer{};
          usb::receive(message_buffer)) {
        switch (message_buffer.id) {
        case usb::Message_identifier::M90E26_ReadRequest: {
          auto msg = M90E26_register_transfer{static_cast<M90E26_register>(
              message_buffer.payload.read_request.address.v)};
          if (spi_read(u1_spi, msg)) {
            usb::send(usb::M90E26_read_response{
                i8{0},
                {message_buffer.payload.read_request.address,
                 u16{msg.rx_value()}}});
          } else {
            usb::send(usb::M90E26_read_response{i8{-1}, {}});
          }
          break;
        }

        case usb::Message_identifier::M90E26_WriteRequest: {
          auto msg = M90E26_register_transfer{
              static_cast<M90E26_register>(
                  message_buffer.payload.write_request.reg.address.v),
              message_buffer.payload.write_request.reg.value.v};
          if (spi_write(u1_spi, msg)) {
            usb::send(usb::M90E26_write_response{
                i8{0},
                {message_buffer.payload.write_request.reg.address,
                 message_buffer.payload.write_request.reg.value}});
          } else {
            usb::send(usb::M90E26_write_response{i8{-1}, {}});
          }
          break;
        }

        default:
          break;
        }
      }
    }
  }

  void on_u1_zx_edge() { relay.force(); }

} // namespace belmoor

extern "C" void mainloop() { belmoor::mainloop(); }

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == U1_ZX_Pin) {
    belmoor::on_u1_zx_edge();
  }
}
