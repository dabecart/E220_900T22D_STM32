/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
void SystemClock_Config(void);
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
#define TFT_CS_Pin GPIO_PIN_6
#define TFT_CS_GPIO_Port GPIOA
#define TFT_A0_Pin GPIO_PIN_4
#define TFT_A0_GPIO_Port GPIOC
#define TFT_RESET_Pin GPIO_PIN_0
#define TFT_RESET_GPIO_Port GPIOB
#define LORA_M1_Pin GPIO_PIN_15
#define LORA_M1_GPIO_Port GPIOB
#define TEST_LED_Pin GPIO_PIN_6
#define TEST_LED_GPIO_Port GPIOC
#define LORA_M0_Pin GPIO_PIN_8
#define LORA_M0_GPIO_Port GPIOA
#define LORA_AUX_Pin GPIO_PIN_15
#define LORA_AUX_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
