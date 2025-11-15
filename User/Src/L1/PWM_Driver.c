/**
 * @file    PWM_Driver.c
 *
 * @brief   Timer initialization and PWM generation
 *
 * Created on: November 13, 2025
 */

#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "main.h"
#include "L1/PWM_Driver.h"

extern TIM_HandleTypeDef htim1;
extern QueueHandle_t PWM_Queue;

/**
 * @brief Control PWM timers for servo control
 *
 * Configures PWM timers on TIM1 CH1 and CH2 at 50 Hz
 * Waits for queue messages to update PWM duty cycles
 * Duty cycle specified as percentage (0-100%) corresponding to 1 ms to 2 ms pulse width
 */
void PWM_Timer_Task(void *pvParameters)
{
    /* Timer1 is clocked from APB2 @ 84 MHz */
    /* Make sure external clock from STLink is timer source - HSI has a lot of drift */

    /* Maximum Counter period is 65535
     * Prescaler = (Timer clock / Desired counter clock) - 1
     * Prescaler = (84,000,000 / 1,000,000) - 1 = 83
     * Counter clock = 1 MHz
     * PWM Frequency = Counter clock / (Period + 1) = 1,000,000 / 20000 = 50 Hz
     *
     * Configuration set in CubeMX
     */

    /* Initialize PWM duty cycles to 50% (1.5 ms pulse width) */
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 1500);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 1500);

    /* Start PWM generation on TIM1 CH1 and CH2 */
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);

    PWM_Duty_Cycle_t pwm_msg;

    while (true)
    {
        /* Wait for new PWM duty cycle from queue */

        if (xQueueReceive(PWM_Queue, &pwm_msg, portMAX_DELAY) == pdTRUE)
        {
            if (pwm_msg.duty_cycle_percent > 100)
            {
                pwm_msg.duty_cycle_percent = 100; // Cap at 100%
            }

            /* Scale duty cycle percent to timer pulse length from 1 to 2 ms */
            uint32_t pulse_length = 1000 + (pwm_msg.duty_cycle_percent * 10); // 1 ms to 2 ms
            if (pwm_msg.channel == VERTICAL_SERVO_PWM)
            {
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pulse_length);
            }
            else if (pwm_msg.channel == HORIZONTAL_SERVO_PWM)
            {
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pulse_length);
            }
        }
    }

    UNUSED(pvParameters);
}