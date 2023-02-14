#include "spi.hpp"

#include <stm32f0xx_hal.h>

#include <catch2/catch.hpp>

namespace belmoor {

  SCENARIO("querying status from M90E26", "[soft_spi]") {
    GIVEN("") {
      auto uut = Soft_SPI{};
      uut.tick();

      CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin) == GPIO_PIN_SET);
      CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin) == GPIO_PIN_SET);
      CHECK(uut.state() == SPI_state::Idle);

      WHEN("starting a read query") {
        const auto tx = std::array<std::byte, 3>{
            {std::byte{0x81}, std::byte{0x00}, std::byte{0x00}}};
        auto rx = std::array<std::byte, 3>{};
        REQUIRE(uut.read(tx, rx));

        THEN("") {
          CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin) == GPIO_PIN_SET);
          CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin) == GPIO_PIN_SET);
          CHECK(uut.state() == SPI_state::Select);

          uut.tick();

          CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin) == GPIO_PIN_SET);
          CHECK(uut.state() == SPI_state::Data_Setup);

          uut.tick();

          // read/write bit
          CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                == GPIO_PIN_SET);
          CHECK(uut.state() == SPI_state::Data_Hold);

          uut.tick();

          CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin) == GPIO_PIN_SET);
          CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                == GPIO_PIN_SET);
          CHECK(uut.state() == SPI_state::Data_Setup);

          uut.tick();

          // A6
          CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                == GPIO_PIN_RESET);
          CHECK(uut.state() == SPI_state::Data_Hold);

          uut.tick();

          CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin) == GPIO_PIN_SET);
          CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                == GPIO_PIN_RESET);
          CHECK(uut.state() == SPI_state::Data_Setup);

          uut.tick();

          // A5
          CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                == GPIO_PIN_RESET);
          CHECK(uut.state() == SPI_state::Data_Hold);

          uut.tick();

          CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin) == GPIO_PIN_SET);
          CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                == GPIO_PIN_RESET);
          CHECK(uut.state() == SPI_state::Data_Setup);

          uut.tick();

          // A4
          CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                == GPIO_PIN_RESET);
          CHECK(uut.state() == SPI_state::Data_Hold);

          uut.tick();

          CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin) == GPIO_PIN_SET);
          CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                == GPIO_PIN_RESET);
          CHECK(uut.state() == SPI_state::Data_Setup);

          uut.tick();

          // A3
          CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                == GPIO_PIN_RESET);
          CHECK(uut.state() == SPI_state::Data_Hold);

          uut.tick();

          CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin) == GPIO_PIN_SET);
          CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                == GPIO_PIN_RESET);
          CHECK(uut.state() == SPI_state::Data_Setup);

          uut.tick();

          // A2
          CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                == GPIO_PIN_RESET);
          CHECK(uut.state() == SPI_state::Data_Hold);

          uut.tick();

          CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin) == GPIO_PIN_SET);
          CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                == GPIO_PIN_RESET);
          CHECK(uut.state() == SPI_state::Data_Setup);

          uut.tick();

          // A1
          CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                == GPIO_PIN_RESET);
          CHECK(uut.state() == SPI_state::Data_Hold);

          uut.tick();

          CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin) == GPIO_PIN_SET);
          CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                == GPIO_PIN_RESET);
          CHECK(uut.state() == SPI_state::Data_Setup);

          uut.tick();

          // A0
          CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                == GPIO_PIN_SET);
          CHECK(uut.state() == SPI_state::Data_Hold);

          uut.tick();

          CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                == GPIO_PIN_RESET);
          CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin) == GPIO_PIN_SET);
          CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                == GPIO_PIN_SET);
          CHECK(uut.state() == SPI_state::Data_Setup);

          uut.tick();
          // D15
          uut.tick();
          uut.tick();
          // D14
          uut.tick();
          uut.tick();
          // D13
          uut.tick();
          uut.tick();
          // D12
          uut.tick();
          uut.tick();
          // D11
          uut.tick();
          uut.tick();
          // D10
          uut.tick();
          uut.tick();
          // D9
          uut.tick();
          uut.tick();
          // D8
          uut.tick();
          uut.tick();
          // D7
          uut.tick();
          uut.tick();
          // D6
          uut.tick();
          uut.tick();
          // D5
          uut.tick();
          uut.tick();
          // D4
          uut.tick();
          uut.tick();
          // D3
          uut.tick();
          uut.tick();
          // D2
          uut.tick();
          uut.tick();
          // D1
          uut.tick();
          uut.tick();
          // D0
          uut.tick();
          CHECK(uut.state() == SPI_state::Deselect);
          uut.tick();
          CHECK(uut.state() == SPI_state::Idle);

          WHEN("sequential write") {
            REQUIRE(uut.write(tx));

            THEN("") {
              CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                    == GPIO_PIN_SET);
              CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                    == GPIO_PIN_SET);
              CHECK(uut.state() == SPI_state::Select);

              uut.tick();

              CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                    == GPIO_PIN_SET);
              CHECK(uut.state() == SPI_state::Data_Setup);

              uut.tick();

              // A7
              CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                    == GPIO_PIN_SET);
              CHECK(uut.state() == SPI_state::Data_Hold);

              uut.tick();

              CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                    == GPIO_PIN_SET);
              CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                    == GPIO_PIN_SET);
              CHECK(uut.state() == SPI_state::Data_Setup);

              uut.tick();

              // A6
              CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                    == GPIO_PIN_RESET);
              CHECK(uut.state() == SPI_state::Data_Hold);

              uut.tick();

              CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                    == GPIO_PIN_SET);
              CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                    == GPIO_PIN_RESET);
              CHECK(uut.state() == SPI_state::Data_Setup);

              uut.tick();

              // A5
              CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                    == GPIO_PIN_RESET);
              CHECK(uut.state() == SPI_state::Data_Hold);

              uut.tick();

              CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                    == GPIO_PIN_SET);
              CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                    == GPIO_PIN_RESET);
              CHECK(uut.state() == SPI_state::Data_Setup);

              uut.tick();

              // A4
              CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                    == GPIO_PIN_RESET);
              CHECK(uut.state() == SPI_state::Data_Hold);

              uut.tick();

              CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                    == GPIO_PIN_SET);
              CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                    == GPIO_PIN_RESET);
              CHECK(uut.state() == SPI_state::Data_Setup);

              uut.tick();

              // A3
              CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                    == GPIO_PIN_RESET);
              CHECK(uut.state() == SPI_state::Data_Hold);

              uut.tick();

              CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                    == GPIO_PIN_SET);
              CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                    == GPIO_PIN_RESET);
              CHECK(uut.state() == SPI_state::Data_Setup);

              uut.tick();

              // A2
              CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                    == GPIO_PIN_RESET);
              CHECK(uut.state() == SPI_state::Data_Hold);

              uut.tick();

              CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                    == GPIO_PIN_SET);
              CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                    == GPIO_PIN_RESET);
              CHECK(uut.state() == SPI_state::Data_Setup);

              uut.tick();

              // A1
              CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                    == GPIO_PIN_RESET);
              CHECK(uut.state() == SPI_state::Data_Hold);

              uut.tick();

              CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                    == GPIO_PIN_SET);
              CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                    == GPIO_PIN_RESET);
              CHECK(uut.state() == SPI_state::Data_Setup);

              uut.tick();

              // A0
              CHECK(HAL_GPIO_ReadPin(U1_nCS_GPIO_Port, U1_nCS_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_SCK_GPIO_Port, U1_SCK_Pin)
                    == GPIO_PIN_RESET);
              CHECK(HAL_GPIO_ReadPin(U1_MOSI_GPIO_Port, U1_MOSI_Pin)
                    == GPIO_PIN_SET);
              CHECK(uut.state() == SPI_state::Data_Hold);
            }
          }
        }
      }
    }
  }

} // namespace belmoor
