/**
 * @file Calibrate_Mode.c
 *
 * @brief Implements calibration mode operations for the warehouse crane.
 * Contains state machine logic for servo speeds.
 */

/* Module Header */
#include "L4/Calibrate_Mode.h"

/* Standard Libraries */

/* User Libraries */
#include "user_main.h"
#include "L3/Control_Loop.h"

#define HOME_POSITION_MM (60)
#define UPPER_SHELF_POSITION_MM (130)

typedef enum Calibrate_States
{
    STATE_CALIBRATE_START = 0,
    STATE_CALIBRATE_MOVE_VERTICAL_TO_HOME,
    STATE_CALIBRATE_MOVE_VERTICAL_TO_TOP,
    STATE_CALIBRATE_MOVE_VERTICAL_TO_BOTTOM,
    STATE_CALIBRATE_IDLE
} Calibrate_States_t;

extern EventGroupHandle_t Motor_Event_Group;
static Calibrate_States_t calibrate_state;

/**
 * @brief Reset and initialize calibration mode.
 */
void Initialize_Calibrate_Mode(void)
{
    calibrate_state = STATE_CALIBRATE_START;
}

/**
 * @brief Run the calibration mode state machine.
 */
void Run_Calibrate_Mode(void)
{
    switch (calibrate_state)
    {
    case STATE_CALIBRATE_START:
        print_str("Entering calibration mode\r\n");
        /* Enable PID */
        Toggle_PID_Control(true);
        /* Set Setpoint to home position */
        Set_Setpoint(HOME_POSITION_MM);
        /* Clear Wait for Motor Event */
        xEventGroupClearBits(Motor_Event_Group, MOTOR_EVENT_BIT);
        calibrate_state = STATE_CALIBRATE_MOVE_VERTICAL_TO_HOME;
        break;
    case STATE_CALIBRATE_MOVE_VERTICAL_TO_HOME:
        print_str("Calibrating: Moving vertical to home position\r\n");
        /* Wait for vertical movement to reach home position */
        xEventGroupWaitBits(Motor_Event_Group, MOTOR_EVENT_BIT, pdTRUE, pdTRUE, portMAX_DELAY) & MOTOR_EVENT_BIT;
        Set_Setpoint(UPPER_SHELF_POSITION_MM); /* Move to upper shelf position */
        /* Clear Wait for Motor Event */
        xEventGroupClearBits(Motor_Event_Group, MOTOR_EVENT_BIT);
        calibrate_state = STATE_CALIBRATE_MOVE_VERTICAL_TO_TOP;
        break;
    case STATE_CALIBRATE_MOVE_VERTICAL_TO_TOP:
        print_str("Calibrating: Moving vertical to top position\r\n");
        xEventGroupWaitBits(Motor_Event_Group, MOTOR_EVENT_BIT, pdTRUE, pdTRUE, portMAX_DELAY) & MOTOR_EVENT_BIT;
        Set_Setpoint(HOME_POSITION_MM); /* Move to bottom position */
        /* Clear Wait for Motor Event */
        xEventGroupClearBits(Motor_Event_Group, MOTOR_EVENT_BIT);
        calibrate_state = STATE_CALIBRATE_MOVE_VERTICAL_TO_BOTTOM;
        break;
    case STATE_CALIBRATE_MOVE_VERTICAL_TO_BOTTOM:
        print_str("Calibrating: Moving vertical to bottom position\r\n");
        xEventGroupWaitBits(Motor_Event_Group, MOTOR_EVENT_BIT, pdTRUE, pdTRUE, portMAX_DELAY) & MOTOR_EVENT_BIT;
        Set_Setpoint(HOME_POSITION_MM); /* Move to home position */
        /* Clear Wait for Motor Event */
        xEventGroupClearBits(Motor_Event_Group, MOTOR_EVENT_BIT);
        print_str("Calibration complete. Entering idle state.\r\n");
        calibrate_state = STATE_CALIBRATE_IDLE;
        break;
    case STATE_CALIBRATE_IDLE:
        /* Calibration complete, remain idle */
        Toggle_PID_Control(false);
        vTaskDelay(pdMS_TO_TICKS(100));
        break;
    }
}