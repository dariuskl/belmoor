#ifndef SRC_FAKE_USB_CDC_IF_HPP
#define SRC_FAKE_USB_CDC_IF_HPP

#include <cstddef>
#include <queue>

extern std::queue<std::byte> usb_transmit_buffer;

inline void CDC_Transmit_FS(uint8_t *Buf, const uint32_t Len) {
  for (const auto end = Buf + Len; Buf < end; ++Buf) {
    usb_transmit_buffer.push(std::byte{*Buf});
  }
}

#endif // SRC_FAKE_USB_CDC_IF_HPP
