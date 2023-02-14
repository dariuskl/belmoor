/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LD1_RED_Pin GPIO_PIN_0
#define LD1_RED_GPIO_Port GPIOA
#define LD1_GREEN_Pin GPIO_PIN_1
#define LD1_GREEN_GPIO_Port GPIOA
#define U1_nCS_Pin GPIO_PIN_4
#define U1_nCS_GPIO_Port GPIOA
#define U1_SCK_Pin GPIO_PIN_5
#define U1_SCK_GPIO_Port GPIOA
#define U1_MISO_Pin GPIO_PIN_6
#define U1_MISO_GPIO_Port GPIOA
#define U1_MOSI_Pin GPIO_PIN_7
#define U1_MOSI_GPIO_Port GPIOA
#define U1_IRQ_Pin GPIO_PIN_0
#define U1_IRQ_GPIO_Port GPIOB
#define U1_IRQ_EXTI_IRQn EXTI0_1_IRQn
#define U1_ZX_Pin GPIO_PIN_1
#define U1_ZX_GPIO_Port GPIOB
#define U1_ZX_EXTI_IRQn EXTI0_1_IRQn
#define U1_WarnOut_Pin GPIO_PIN_2
#define U1_WarnOut_GPIO_Port GPIOB
#define U1_WarnOut_EXTI_IRQn EXTI2_3_IRQn
#define RL1_nENABLE_Pin GPIO_PIN_11
#define RL1_nENABLE_GPIO_Port GPIOB
#define U1_CF2_Pin GPIO_PIN_4
#define U1_CF2_GPIO_Port GPIOB
#define U1_CF2_EXTI_IRQn EXTI4_15_IRQn
#define U1_CF1_Pin GPIO_PIN_5
#define U1_CF1_GPIO_Port GPIOB
#define U1_CF1_EXTI_IRQn EXTI4_15_IRQn
#define SW1_nPRESSED_Pin GPIO_PIN_7
#define SW1_nPRESSED_GPIO_Port GPIOB
#define SW1_nPRESSED_EXTI_IRQn EXTI4_15_IRQn
#define U1_nRESET_Pin GPIO_PIN_8
#define U1_nRESET_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
