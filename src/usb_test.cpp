#include "usb.hpp"

#include <catch2/catch.hpp>

namespace belmoor {

  SCENARIO("receive write request", "[usb]") {
    GIVEN("a write request") {
      const auto request = std::array<uint8_t, 4>{0x02, 0x30, 0x78, 0x56};

      WHEN("pushing the data in all at once") {
        usb::push_buffer(request.data(), request.size());

        THEN("the message is correctly unpacked") {
          auto message = usb::Message_buffer{};
          REQUIRE(usb::receive(message));
          CHECK(message.id == usb::Message_identifier::M90E26_WriteRequest);
          CHECK(message.payload.write_request.reg.address.v == 0x30);
          CHECK(message.payload.write_request.reg.value.v == 0x5678);
        }
      }
    }
  }

  SCENARIO("send write response", "[usb]") {
    usb::send(usb::M90E26_write_response{{1}, {{0x30}, {0x5678}}});
    REQUIRE_FALSE(usb_transmit_buffer.empty());
    CHECK(usb_transmit_buffer.front() == std::byte{0x03});
    usb_transmit_buffer.pop();
    CHECK(usb_transmit_buffer.front() == std::byte{0x01});
    usb_transmit_buffer.pop();
    CHECK(usb_transmit_buffer.front() == std::byte{0x30});
    usb_transmit_buffer.pop();
    CHECK(usb_transmit_buffer.front() == std::byte{0x78});
    usb_transmit_buffer.pop();
    CHECK(usb_transmit_buffer.front() == std::byte{0x56});
    usb_transmit_buffer.pop();
  }

} // namespace belmoor
