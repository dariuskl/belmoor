#include "usb.hpp"

#include <queue>

namespace belmoor::usb {

  namespace {

    auto message_queue = std::queue<Message_buffer>{};

  }

  enum class Rx_state {
    ID,
    Payload,
  };

  extern "C" void push_buffer(const uint8_t *Buf, uint32_t Len) {
    for (const auto end = Buf + Len; Buf < end; ++Buf) {
      const auto b = std::byte{*Buf};

      static auto state_ = Rx_state::ID;
      static auto message_ = Message_buffer{};
      static auto payload_bytes_received_ = 0;
      static auto length = 0;

      switch (state_) {
      default:
      case Rx_state::ID:
        message_.id = static_cast<Message_identifier>(b);
        switch (message_.id) {
        case Message_identifier::M90E26_ReadRequest:
          state_ = Rx_state::Payload;
          length = 1;
          payload_bytes_received_ = 0;
          break;

        case Message_identifier::M90E26_WriteRequest:
          state_ = Rx_state::Payload;
          length = 3;
          payload_bytes_received_ = 0;
          break;

        default:
          state_ = Rx_state::ID;
          break;
        }
        break;

      case Rx_state::Payload:
        switch (message_.id) {
        case Message_identifier::M90E26_ReadRequest:
          switch (payload_bytes_received_) {
          case 0:
            message_.payload.read_request.address = u8{
                static_cast<uint8_t>(b)};
            break;

          default:
            break;
          }
          break;

        case Message_identifier::M90E26_WriteRequest:
          switch (payload_bytes_received_) {
          case 0:
            message_.payload.write_request.reg.address = u8{
                static_cast<uint8_t>(b)};
            break;

          case 1:
            message_.payload.write_request.reg.value = u16{
                static_cast<uint8_t>(b)};
            break;

          case 2:
            message_.payload.write_request.reg.value
                = message_.payload.write_request.reg.value
                  | u16{static_cast<uint16_t>(static_cast<uint16_t>(b) << 8U)};
            break;

          default:
            break;
          }
          break;

        default:
          state_ = Rx_state::ID;
          break;
        }

        ++payload_bytes_received_;

        if (payload_bytes_received_ >= length) {
          message_queue.push(message_);
          state_ = Rx_state::ID;
        }
        break;
      }
    }
  }

  bool receive(Message_buffer &msg) {
    while (not message_queue.empty()) {
      msg = message_queue.front();
      message_queue.pop();
      return true;
    }
    return false;
  }

} // namespace belmoor::usb
