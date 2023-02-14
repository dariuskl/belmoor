#include "app.hpp"

#include "device.hpp"
#include "m90e26.hpp"
#include "nvm.hpp"
#include "spi.hpp"
#include "sys_tick.hpp"
#include "usb.hpp"
#include "utils.hpp"

#include <main.h>

namespace belmoor {

  // Because the metering IC is powered independently of the MCU, we have to
  // handle unsolicited resets.
  enum class Operating_mode {
    Offline, // Communication with the metering IC cannot be established. This
             // is normally the case, if mains voltage is not connected and thus
             // there is no primary-side power.
    SetUp,   // The metering IC cannot be used for measurements as there is no
             // calibration data available.
    Normal,  // The metering IC is calibrated and measurements are being done.
    Calibration // The metering IC is in calibration mode.
  };

  // To show more information on the screen that the little that fits
  // simultaneously, additional data is alternated with the period defined
  // here.
  constexpr auto AdditionalInfo_Period = Duration{1000}; // ms

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

  Operating_mode get_next_operating_mode(const Operating_mode current,
                                         std::optional<uint16_t> status,
                                         std::optional<uint16_t> adj_start) {
    switch (current) {
    default:
    case Operating_mode::Offline:
      if (status and adj_start and (*adj_start != 0xffffU)) {
        return Operating_mode::SetUp;
      } else {
        return Operating_mode::Offline;
      }

    case Operating_mode::SetUp:
      if ((not status) or (not adj_start) or (*adj_start == 0xffffU)) {
        return Operating_mode::Offline;
      } else if (*adj_start == M90E26_AdjStart_Measurement) {
        return Operating_mode::Normal;
      } else if (*adj_start == M90E26_AdjStart_Adjustment) {
        return Operating_mode::Calibration;
      } else {
        return Operating_mode::SetUp;
      }

    case Operating_mode::Normal:
      if ((not status) or (not adj_start) or (*adj_start == 0xffffU)) {
        return Operating_mode::Offline;
      } else if (*adj_start == M90E26_AdjStart_NoMeasurement) {
        return Operating_mode::SetUp;
      } else if (*adj_start == M90E26_AdjStart_Adjustment) {
        return Operating_mode::Calibration;
      } else {
        return Operating_mode::Normal;
      }

    case Operating_mode::Calibration:
      if ((not status) or (not adj_start) or (*adj_start == 0xffffU)) {
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

  struct Measurements {
    std::optional<uint16_t> status;
    std::optional<uint16_t> adj_start;
    std::optional<Fixed_point<1000>> il_rms;
    std::optional<Fixed_point<100>> u_rms;
    std::optional<Fixed_point<1000>> pl_mean;
    std::optional<Fixed_point<1000>> sl_mean;
    std::optional<Fixed_point<1000>> ql_mean;
    std::optional<Fixed_point<100>> freq;
    std::optional<Fixed_point<1000>> pfl;
  };

  void led_step(const Operating_mode operating_mode, const Measurements &m,
                const Relay &relay) {
    switch (operating_mode) {
    case Operating_mode::Offline:
    case Operating_mode::SetUp:
      HAL_GPIO_WritePin(LD1_RED_GPIO_Port, LD1_RED_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(LD1_GREEN_GPIO_Port, LD1_GREEN_Pin, GPIO_PIN_RESET);
      break;

    case Operating_mode::Normal:
      if (relay.closed()) {
        HAL_GPIO_WritePin(LD1_GREEN_GPIO_Port, LD1_GREEN_Pin, GPIO_PIN_SET);
      } else {
        HAL_GPIO_WritePin(LD1_GREEN_GPIO_Port, LD1_GREEN_Pin, GPIO_PIN_RESET);
      }
      if (*(m.status) != 0U) {
        HAL_GPIO_WritePin(LD1_RED_GPIO_Port, LD1_RED_Pin, GPIO_PIN_SET);
      } else {
        HAL_GPIO_WritePin(LD1_RED_GPIO_Port, LD1_RED_Pin, GPIO_PIN_RESET);
      }
      break;

    case Operating_mode::Calibration:
      HAL_GPIO_WritePin(LD1_RED_GPIO_Port, LD1_RED_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LD1_GREEN_GPIO_Port, LD1_GREEN_Pin, GPIO_PIN_RESET);
      break;
    }
  }

  void display_step(const Duration period, Display &display,
                    const Operating_mode operating_mode, const Measurements &m,
                    const Relay &relay) {
    static auto additional_info_timer = AdditionalInfo_Period;
    static auto additional_info_index = 0;

    display.clear();

    switch (operating_mode) {
    case Operating_mode::Offline:
      // If there is no mains, there will never be a zero crossing and also
      // no risk of an arc, so the relay may be updated immediately.
      display.set_font(Font_11x18)
          .at(0, 0)
          .print("Mains not\nconnected")
          .set_font(Font_7x10)
          .at(0, 40);
      break;

    case Operating_mode::SetUp:
      display.set_font(Font_11x18)
          .at(0, 0)
          .print("Not\ncalibrated")
          .set_font(Font_7x10)
          .at(0, 40);
      switch (additional_info_index) {
      default:
      case 0:
      case 1:
        display.print("Status: ", u16{*(m.status)});
        break;

      case 2:
      case 3:
        display.print("AdjStart: ", u16{*(m.adj_start)});
        break;
      }
      break;

    case Operating_mode::Normal:
      if (*(m.status) != 0U) {
        display.set_font(Font_11x18)
            .at(0, 0)
            .print("Error")
            .set_font(Font_7x10)
            .at(0, 20)
            .print("status: ", m.status);
      } else {
        display.set_font(Font_11x18)
            .at(0, 0)
            .print(m.u_rms, " V")
            .at(0, 20)
            .print(m.il_rms, " A")
            .set_font(Font_7x10)
            .at(0, 40)
            .print(m.pl_mean, " kW")
            .at(64, 40)
            .print(m.freq, " Hz")
            .at(0, 51)
            .print(m.sl_mean, " kVA")
            .at(64, 51)
            .print(m.pfl);
      }
      break;

    case Operating_mode::Calibration:
      display.set_font(Font_11x18)
          .at(0, 0)
          .print(m.u_rms, " V")
          .at(0, 20)
          .print(m.il_rms, " A");
      break;
    }

    if (additional_info_timer <= 0) {
      if (additional_info_index < 3) {
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
      display.at(0, 63).line_to(87, 63).line_to(103, 55).at(103, 63).line_to(
          127, 63);
    }

    display.update();
  }

  const Relay &relay_step(Relay &relay, const Operating_mode operating_mode) {
    if (operating_mode == Operating_mode::Offline) {
      relay.update();
    }
    return relay;
  }

  void usb_step(Soft_SPI &u1_spi) {
    if (auto message_buffer = usb::Message_buffer{};
        usb::receive(message_buffer)) {
      switch (message_buffer.id) {
      case usb::Message_identifier::M90E26_ReadRequest: {
        if (auto msg = M90E26_register_transfer{static_cast<M90E26_register>(
                message_buffer.payload.read_request.address.v)};
            spi_read(u1_spi, msg)) {
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
        if (auto msg
            = M90E26_register_transfer{static_cast<M90E26_register>(
                                           message_buffer.payload.write_request
                                               .reg.address.v),
                                       message_buffer.payload.write_request.reg
                                           .value.v};
            spi_write(u1_spi, msg)) {
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

  Measurements measurement_step(Soft_SPI &u1_spi) {
    auto m = Measurements{};
    if (auto st = read_status(SPI_read{u1_spi}); st) {
      m.status = st;

      if (auto msg = M90E26_register_transfer{M90E26_register::AdjStart};
          spi_read(u1_spi, msg)) {
        m.adj_start = msg.rx_value();
      }

      m.pfl = read_power_factor(SPI_read{u1_spi});
      m.freq = read_frequency(SPI_read{u1_spi});
      m.il_rms = read_current(SPI_read{u1_spi});
      m.u_rms = read_voltage(SPI_read{u1_spi});
      m.pl_mean = read_real_power(SPI_read{u1_spi});
      m.sl_mean = read_apparent_power(SPI_read{u1_spi});
      m.ql_mean = read_reactive_power(SPI_read{u1_spi});
    }
    return m;
  }

  Operating_mode
  operating_mode_step(const Operating_mode &previous_operating_mode,
                      const Measurements &m, Soft_SPI &u1_spi,
                      const std::optional<Persistent_data> &parameters) {
    const auto next_operating_mode = get_next_operating_mode(
        previous_operating_mode, m.status, m.adj_start);

    if ((previous_operating_mode == Operating_mode::Offline)
        and (next_operating_mode != Operating_mode::Offline)) {
      auto metering_regs = M90E26_metering_registers{};
      metering_regs.MMode = static_cast<uint16_t>(metering_regs.MMode
                                                  & ~M90E26_MMode_Lgain_Mask)
                            | M90E26_MMode_Lgain_16;
      auto msg = M90E26_register_transfer{M90E26_register::MMode,
                                          metering_regs.MMode};
      spi_write(u1_spi, msg);
      msg = M90E26_register_transfer{M90E26_register::CS1, metering_regs.cs1()};
      spi_write(u1_spi, msg);

      if (parameters) {
        msg = M90E26_register_transfer{M90E26_register::U_gain,
                                       parameters->u1_settings.Ugain};
        spi_write(u1_spi, msg);
        msg = M90E26_register_transfer{M90E26_register::I_gain_L,
                                       parameters->u1_settings.IgainL};
        spi_write(u1_spi, msg);
        msg = M90E26_register_transfer{M90E26_register::I_offset_L,
                                       parameters->u1_settings.IoffsetL};
        spi_write(u1_spi, msg);
        msg = M90E26_register_transfer{M90E26_register::CS2,
                                       parameters->u1_settings.cs2()};
        spi_write(u1_spi, msg);
        msg = M90E26_register_transfer{M90E26_register::AdjStart,
                                       M90E26_AdjStart_Measurement};
        spi_write(u1_spi, msg);
      }
    } else if ((previous_operating_mode == Operating_mode::Calibration)
               and (next_operating_mode == Operating_mode::Normal)) {
      auto msg = M90E26_register_transfer{M90E26_register::U_gain};
      spi_read(u1_spi, msg);
      const auto ugain = msg.rx_value();
      msg = M90E26_register_transfer{M90E26_register::I_gain_L};
      spi_read(u1_spi, msg);
      const auto igainl = msg.rx_value();
      msg = M90E26_register_transfer{M90E26_register::I_offset_L};
      spi_read(u1_spi, msg);
      const auto ioffsetl = msg.rx_value();
      store(Persistent_data{{{false}}, {ugain, igainl, 0x7530U, 0U, ioffsetl}});
    }

    return next_operating_mode;
  }

  void mainloop() {
    static auto u1_spi = Soft_SPI{};
    htim2.PeriodElapsedCallback = [](TIM_HandleTypeDef *) { u1_spi.tick(); };

    const auto parameters = restore();

    auto display = Display{};
    auto relay = Relay{parameters
                       && parameters->device.options.relay_normally_open};
    auto previous_operating_mode = Operating_mode::Offline;

    on_u1_zx_edge = [&] { relay.update(); };
    on_sw1_falling = [&] { relay.toggle(); };

    prove_out(display);

    relay.close();

    auto sys_tick = Sys_tick{};
    while (true) {
      const auto period = sys_tick();

      const auto measurements = measurement_step(u1_spi);
      const auto operating_mode = operating_mode_step(
          previous_operating_mode, measurements, u1_spi, parameters);
      const auto relay_state = relay_step(relay, operating_mode);
      led_step(operating_mode, measurements, relay_state);
      display_step(period, display, operating_mode, measurements, relay_state);
      usb_step(u1_spi);

      previous_operating_mode = operating_mode;
    }
  }

} // namespace belmoor
