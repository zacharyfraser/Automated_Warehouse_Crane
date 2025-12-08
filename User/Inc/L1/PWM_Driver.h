/**
 * @file    PWM_Driver.h
 *
 * @brief   Header file for PWM_Driver.c
 *
 * Created on: November 13, 2025
 */

#ifndef PWM_DRIVER_H
#define PWM_DRIVER_H

#include <stdint.h>

typedef enum PWM_CHANNEL
{
    VERTICAL_SERVO_PWM = 1,
    HORIZONTAL_SERVO_PWM = 2
} PWM_Channel_t;

typedef enum PWM_DIRECTION
{
    DIRECTION_CLOCKWISE = 1,
    DIRECTION_COUNTERCLOCKWISE = -1,
    DIRECTION_IDLE = 0
} PWM_Direction_t;

typedef struct PWM_DUTY_CYCLE
{
    PWM_Channel_t channel;
    PWM_Direction_t direction;
    uint16_t duty_cycle; /* 0 to 100 percent */
} PWM_Duty_Cycle_t;

void PWM_Timer_Task(void *pvParameters);

#endif /* PWM_DRIVER_H */