/**
 * @file    Button_Driver.c
 *
 * @brief   Button driver implementation
 *
 * Used to operate Manual Mode of the crane.
 * Supports actuation button and direction switch for each axis.
 */

/* Module Header */
#include "L1/Button_Driver.h"

/* Standard Libraries */

/* User Libraries */
#include "user_main.h"
#include "Button_Driver.h"

/**
 * @brief Read horizontal direction switch state
 * @return Button_Actuation_t DIRECTION_H_CLOCKWISE or DIRECTION_H_COUNTERCLOCKWISE
 */
Button_Actuation_t Read_Horizontal_Switch(void)
{
    if (HAL_GPIO_ReadPin(SWITCH_H_GPIO_Port, SWITCH_H_Pin) == GPIO_PIN_SET)
    {
        return BUTTON_PRESSED;
    }
    else
    {
        return BUTTON_NOT_PRESSED;
    }
}

/**
 * @brief Read vertical direction switch state
 * @return Button_Actuation_t DIRECTION_V_UP or DIRECTION_V_DOWN
 */
Button_Actuation_t Read_Vertical_Switch(void)
{
    if (HAL_GPIO_ReadPin(SWITCH_V_GPIO_Port, SWITCH_V_Pin) == GPIO_PIN_SET)
    {
        return BUTTON_PRESSED;
    }
    else
    {
        return BUTTON_NOT_PRESSED;
    }
}

/**
 * @brief Read horizontal actuation button state
 * @return Button_Actuation_t BUTTON_PRESSED or BUTTON_NOT_PRESSED
 */
Button_Actuation_t Read_Horizontal_Button(void)
{
    if (HAL_GPIO_ReadPin(BUTTON_H_GPIO_Port, BUTTON_H_Pin) == GPIO_PIN_SET)
    {
        return BUTTON_PRESSED;
    }
    else
    {
        return BUTTON_NOT_PRESSED;
    }
}

/**
 * @brief Read vertical actuation button state
 * @return Button_Actuation_t BUTTON_PRESSED or BUTTON_NOT_PRESSED
 */
Button_Actuation_t Read_Vertical_Button(void)
{
    if (HAL_GPIO_ReadPin(BUTTON_V_GPIO_Port, BUTTON_V_Pin) == GPIO_PIN_SET)
    {
        return BUTTON_PRESSED;
    }
    else
    {
        return BUTTON_NOT_PRESSED;
    }
}