#ifndef SRC_TEST_SSD1306_H
#define SRC_TEST_SSD1306_H

struct FontDef {
  int FontHeight;
};

extern FontDef Font_6x8;

enum SSD1306_COLOR {
  Black,
  White
};

struct SSD1306_VERTEX {
  uint8_t x;
  uint8_t y;
};

void ssd1306_Init();
void ssd1306_SetDisplayOn(int);
void ssd1306_Fill(SSD1306_COLOR);
void ssd1306_SetCursor(uint8_t, uint8_t);
SSD1306_VERTEX ssd1306_GetCursor();
void ssd1306_WriteString(const char*, FontDef, SSD1306_COLOR);
void ssd1306_Line(uint8_t, uint8_t, uint8_t, uint8_t, SSD1306_COLOR);
void ssd1306_UpdateScreen();

#endif // SRC_TEST_SSD1306_H
