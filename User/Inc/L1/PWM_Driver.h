/**
 * @file    PWM_Driver.h
 *
 * @brief   Header file for PWM_Driver.c
 *
 * Created on: November 13, 2025
 */

#ifndef PWM_DRIVER_H
#define PWM_DRIVER_H

typedef enum PWM_CHANNEL
{
    VERTICAL_SERVO_PWM = 1,
    HORIZONTAL_SERVO_PWM = 2
} PWM_Channel_t;

typedef struct PWM_DUTY_CYCLE
{
    PWM_Channel_t channel;
    uint8_t pulse_width_percent; /* 0 - 100% of full scale servo pulse_width (1 ms to 2 ms pulse width) */
} PWM_Duty_Cycle_t;

void PWM_Timer_Task(void *pvParameters);

#endif /* PWM_DRIVER_H */