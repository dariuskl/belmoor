#ifndef BELMOOR_USB_HPP_
#define BELMOOR_USB_HPP_ 1

#include "pack.hpp"
#include "utils.hpp"

#include <usbd_cdc_if.h>

#include <array>
#include <cstddef>
#include <span>

namespace belmoor::usb {

  enum class Message_identifier {
    M90E26_ReadRequest,
    M90E26_ReadResponse,
    M90E26_WriteRequest,
    M90E26_WriteResponse,
  };

  inline int pack(std::span<std::byte> buffer, const Message_identifier v) {
    return pack(buffer, u8{static_cast<uint8_t>(v)});
  }

  struct M90E26_read_request {
    static constexpr auto ID = Message_identifier::M90E26_ReadRequest;
    u8 address;
  };

  struct M90E26_register {
    u8 address;
    u16 value;
  };

  struct M90E26_read_response {
    static constexpr auto ID = Message_identifier::M90E26_ReadResponse;
    i8 result;
    M90E26_register reg;
  };

  inline int pack(std::span<std::byte> buffer, const M90E26_read_response v) {
    return pack(buffer, M90E26_read_response::ID, v.result, v.reg.address,
                v.reg.value);
  }

  struct M90E26_write_request {
    static constexpr auto ID = Message_identifier::M90E26_WriteRequest;
    M90E26_register reg;
  };

  struct M90E26_write_response {
    static constexpr auto ID = Message_identifier::M90E26_WriteResponse;
    i8 result;
    M90E26_register reg;
  };

  inline int pack(std::span<std::byte> buffer, const M90E26_write_response v) {
    return pack(buffer, M90E26_write_response::ID, v.result, v.reg.address,
                v.reg.value);
  }

  struct Message_buffer {
    Message_identifier id;
    union {
      M90E26_read_request read_request;
      M90E26_read_response read_response;
      M90E26_write_request write_request;
      M90E26_write_response write_response;
    } payload;
  };

  extern "C" void push_buffer(const uint8_t *Buf, uint32_t Len);

  template <typename... Args_> void send(const Args_ &...args) {
    static auto buffer = std::array<std::byte, 32>{};
    CDC_Transmit_FS(static_cast<uint8_t *>(static_cast<void *>(buffer.data())),
                    static_cast<uint16_t>(pack(buffer, args...)));
  }

  bool receive(Message_buffer &msg);

} // namespace belmoor::usb

#endif // BELMOOR_USB_HPP_
