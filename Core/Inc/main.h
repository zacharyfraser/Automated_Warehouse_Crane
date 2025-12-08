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
#define LIM_SW_R_Pin GPIO_PIN_13
#define LIM_SW_R_GPIO_Port GPIOC
#define LIM_SW_R_EXTI_IRQn EXTI15_10_IRQn
#define ULTRASONIC_TRIGGER_Pin GPIO_PIN_0
#define ULTRASONIC_TRIGGER_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define ULTRASONIC_ECHO_Pin GPIO_PIN_6
#define ULTRASONIC_ECHO_GPIO_Port GPIOA
#define BUTTON_H_Pin GPIO_PIN_6
#define BUTTON_H_GPIO_Port GPIOC
#define SWITCH_H_Pin GPIO_PIN_8
#define SWITCH_H_GPIO_Port GPIOC
#define SWITCH_V_Pin GPIO_PIN_9
#define SWITCH_V_GPIO_Port GPIOC
#define VERTICAL_SERVO_PWM_Pin GPIO_PIN_8
#define VERTICAL_SERVO_PWM_GPIO_Port GPIOA
#define HORIZONTAL_SERVO_PWM_Pin GPIO_PIN_9
#define HORIZONTAL_SERVO_PWM_GPIO_Port GPIOA
#define LIM_SW_HIGH_Pin GPIO_PIN_10
#define LIM_SW_HIGH_GPIO_Port GPIOA
#define LIM_SW_HIGH_EXTI_IRQn EXTI15_10_IRQn
#define LIM_SW_LOW_Pin GPIO_PIN_11
#define LIM_SW_LOW_GPIO_Port GPIOA
#define LIM_SW_LOW_EXTI_IRQn EXTI15_10_IRQn
#define LIM_SW_L_Pin GPIO_PIN_12
#define LIM_SW_L_GPIO_Port GPIOA
#define LIM_SW_L_EXTI_IRQn EXTI15_10_IRQn
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define BUTTON_V_Pin GPIO_PIN_8
#define BUTTON_V_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
