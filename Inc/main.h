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
#include "stm32f4xx_hal.h"

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
#define KRow_3_Pin GPIO_PIN_3
#define KRow_3_GPIO_Port GPIOE
#define KRow_2_Pin GPIO_PIN_4
#define KRow_2_GPIO_Port GPIOE
#define KRow_1_Pin GPIO_PIN_13
#define KRow_1_GPIO_Port GPIOC
#define KCol_4_Pin GPIO_PIN_1
#define KCol_4_GPIO_Port GPIOF
#define KCol_3_Pin GPIO_PIN_3
#define KCol_3_GPIO_Port GPIOF
#define KCol_2_Pin GPIO_PIN_5
#define KCol_2_GPIO_Port GPIOF
#define KCol_1_Pin GPIO_PIN_7
#define KCol_1_GPIO_Port GPIOF
#define KRow_4_Pin GPIO_PIN_1
#define KRow_4_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
