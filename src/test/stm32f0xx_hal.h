#ifndef SRC_FAKE_STM32F0XX_HAL_H
#define SRC_FAKE_STM32F0XX_HAL_H

#include <array>
#include <cstdint>

inline void HAL_Delay(uint32_t) {}
inline uint32_t HAL_GetTick() { return 0U; }

extern std::array<uint32_t, 5> GPIO;

inline void HAL_GPIO_WritePin(char port, uint16_t pin, bool high) {
  if (high) {
    GPIO[static_cast<size_t>(port - 'A')] |= pin;
  } else {
    GPIO[static_cast<size_t>(port - 'A')] &= ~pin;
  }
}

inline bool HAL_GPIO_ReadPin(char port, uint16_t pin) {
  return (GPIO[static_cast<size_t>(port - 'A')] & pin) != 0U;
}

constexpr auto HAL_OK = 0;

#define GPIOA 'A'
#define GPIOB 'B'

#define GPIO_PIN_0 ((uint16_t)0x0001U)   /* Pin 0 selected    */
#define GPIO_PIN_1 ((uint16_t)0x0002U)   /* Pin 1 selected    */
#define GPIO_PIN_2 ((uint16_t)0x0004U)   /* Pin 2 selected    */
#define GPIO_PIN_3 ((uint16_t)0x0008U)   /* Pin 3 selected    */
#define GPIO_PIN_4 ((uint16_t)0x0010U)   /* Pin 4 selected    */
#define GPIO_PIN_5 ((uint16_t)0x0020U)   /* Pin 5 selected    */
#define GPIO_PIN_6 ((uint16_t)0x0040U)   /* Pin 6 selected    */
#define GPIO_PIN_7 ((uint16_t)0x0080U)   /* Pin 7 selected    */
#define GPIO_PIN_8 ((uint16_t)0x0100U)   /* Pin 8 selected    */
#define GPIO_PIN_9 ((uint16_t)0x0200U)   /* Pin 9 selected    */
#define GPIO_PIN_10 ((uint16_t)0x0400U)  /* Pin 10 selected   */
#define GPIO_PIN_11 ((uint16_t)0x0800U)  /* Pin 11 selected   */
#define GPIO_PIN_12 ((uint16_t)0x1000U)  /* Pin 12 selected   */
#define GPIO_PIN_13 ((uint16_t)0x2000U)  /* Pin 13 selected   */
#define GPIO_PIN_14 ((uint16_t)0x4000U)  /* Pin 14 selected   */
#define GPIO_PIN_15 ((uint16_t)0x8000U)  /* Pin 15 selected   */
#define GPIO_PIN_All ((uint16_t)0xFFFFU) /* All pins selected */

#define GPIO_PIN_SET true
#define GPIO_PIN_RESET false

typedef struct {
  uint32_t DR;
  uint8_t IDR;
  uint32_t CR;
  uint32_t INIT;
  uint32_t POL;
} CRC_TypeDef;

#define CRC_CR_RESET 0U

extern CRC_TypeDef *const CRC;

typedef struct {
  uint32_t ACR;
  uint32_t KEYR;
  uint32_t OPTKEYR;
  uint32_t SR;
  uint32_t CR;
  uint32_t AR;
  uint32_t RESERVED;
  uint32_t OBR;
  uint32_t WRPR;
} FLASH_TypeDef;

constexpr auto FLASH_CR_LOCK = 1U << 7U;

extern FLASH_TypeDef *const FLASH;

#define FLASH_PAGE_SIZE 2048

#endif // SRC_FAKE_STM32F0XX_HAL_H
