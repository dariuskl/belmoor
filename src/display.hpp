
#ifndef BELMOOR_DISPLAY_HPP_
#define BELMOOR_DISPLAY_HPP_ 1

#include "print.hpp"

#include <ssd1306.h>

#include <array>
#include <cstring>

namespace belmoor {

  class Display {
    std::array<char, 64> buf_{};
    FontDef font_{Font_6x8};
    SSD1306_COLOR color_{White};

   public:
    Display() {
      ssd1306_Init();
      ssd1306_SetDisplayOn(1U);
    }

    Display &set_color(const SSD1306_COLOR color) {
      color_ = color;
      return *this;
    }

    Display &set_font(const FontDef font) {
      font_ = font;
      return *this;
    }

    Display &fill() {
      ssd1306_Fill(White);
      return *this;
    }

    Display &clear() {
      ssd1306_Fill(Black);
      return *this;
    }

    Display &at(const int x, const int y) {
      ssd1306_SetCursor(static_cast<uint8_t>(x), static_cast<uint8_t>(y));
      return *this;
    }

    template <typename... Args> Display &print(Args... args) {
      belmoor::print(buf_, args...);
      const auto anchor = ssd1306_GetCursor();
      for (char *s = std::strtok(buf_.data(), "\n"); s;
           s = std::strtok(nullptr, "\n"),
                at(anchor.x, ssd1306_GetCursor().y + font_.FontHeight)) {
        ssd1306_WriteString(s, font_, color_);
      }
      return *this;
    }

    Display &line_to(const int x, const int y) {
      const auto start = ssd1306_GetCursor();
      ssd1306_Line(start.x, start.y, static_cast<uint8_t>(x),
                   static_cast<uint8_t>(y), color_);
      ssd1306_SetCursor(static_cast<uint8_t>(x), static_cast<uint8_t>(y));
      return *this;
    }

    Display &update() {
      ssd1306_UpdateScreen();
      return *this;
    }
  };

} // namespace belmoor

#endif // BELMOOR_DISPLAY_HPP_
