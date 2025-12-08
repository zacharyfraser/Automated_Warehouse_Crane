/**
 * @file    Limit_Switch_Driver.c
 *
 * @brief   Limit switch driver implementation
 *
 * When any switch is triggered, the PWM driver is disabled
 *
 * Switch EXTI: 5
 *
 * Created on: November 13, 2025
 */

/* Module Header */
#include "L1/Limit_Switch_Driver.h"

/* Standard Libraries */

/* User Libraries */
#include "user_main.h"
#include "L1/PWM_Driver.h"

/**
 * @brief EXTI line detection callbacks
 *
 * This function is called by the HAL library on an external interrupt.
 * It disables all PWM outputs when a limit switch is triggered.
 *
 * @param GPIO_Pin Specifies the pins connected to the EXTI line
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == LIM_SW_R_Pin || GPIO_Pin == LIM_SW_L_Pin ||
        GPIO_Pin == LIM_SW_HIGH_Pin || GPIO_Pin == LIM_SW_LOW_Pin)
    {
        /* Disable all PWM outputs */
        PWM_Disable_All();
    }
}