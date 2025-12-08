/**
 * @file    Button_Driver.h
 *
 * @brief   Header file for Button_Driver.c
 *
 * Created on: November 13, 2025
 */

#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

#include <stdint.h>

typedef enum Horizontal_Direction
{
    DIRECTION_H_CLOCKWISE = 1,
    DIRECTION_H_COUNTERCLOCKWISE = -1,
    DIRECTION_H_IDLE = 0
} Horizontal_Direction_t;

typedef enum Vertical_Direction
{
    DIRECTION_V_UP = 1,
    DIRECTION_V_DOWN = -1,
    DIRECTION_V_IDLE = 0
} Vertical_Direction_t;

typedef enum Button_Actuation
{
    BUTTON_NOT_PRESSED = 0,
    BUTTON_PRESSED = 1
} Button_Actuation_t;

Button_Actuation_t Read_Horizontal_Switch(void);
Button_Actuation_t Read_Vertical_Switch(void);
Button_Actuation_t Read_Horizontal_Button(void);
Button_Actuation_t Read_Vertical_Button(void);

#endif /* BUTTON_DRIVER_H */
