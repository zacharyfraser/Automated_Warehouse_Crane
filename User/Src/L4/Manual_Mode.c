/**
 * @file Manual_Mode.c
 *
 * @brief Implements manual mode operations for the warehouse crane.
 * Contains state machine logic for manual control via switches and buttons.
 */

/* Module Header */
#include "L4/Manual_Mode.h"

/* Standard Libraries */

/* User Libraries */
#include "user_main.h"
#include "L1/Button_Driver.h"
#include "L1/PWM_Driver.h"
#include "L3/Control_Loop.h"

typedef enum Manual_States
{
    STATE_MANUAL_IDLE = 0,
    STATE_MANUAL_CONTROL
} Manual_States_t;

extern QueueHandle_t PWM_Queue;

static Manual_States_t manual_state;

static void Manual_Control(void);
static void Set_Horizontal_Movement(Horizontal_Direction_t direction);
static void Set_Vertical_Movement(Vertical_Direction_t direction);

/**
 * @brief Reset and initialize manual mode.
 */
void Initialize_Manual_Mode(void)
{
    manual_state = STATE_MANUAL_IDLE;
    Toggle_PID_Control(false); /* Disable PID control loop */
}

/**
 * @brief Run the manual mode state machine.
 */
void Run_Manual_Mode(void)
{
    switch (manual_state)
    {
    case STATE_MANUAL_IDLE:
        manual_state = STATE_MANUAL_CONTROL;
        break;

    case STATE_MANUAL_CONTROL:
        Manual_Control();
        break;

    default:
        manual_state = STATE_MANUAL_IDLE;
        break;
    }
    vTaskDelay(pdMS_TO_TICKS(20)); /* Run at 50 Hz */
}

/**
 * @brief Handle manual control operations.
 * Reads switch and button states to control crane movements.
 */
static void Manual_Control(void)
{
    static Horizontal_Direction_t h_direction = DIRECTION_H_IDLE;
    static Vertical_Direction_t v_direction = DIRECTION_V_IDLE;

    /* Read horizontal direction switch */
    if (Read_Horizontal_Switch() == BUTTON_PRESSED)
    {
        h_direction = DIRECTION_H_CLOCKWISE;
    }
    else
    {
        h_direction = DIRECTION_H_COUNTERCLOCKWISE;
    }
    /* Read vertical direction switch */
    if (Read_Vertical_Switch() == BUTTON_PRESSED)
    {
        v_direction = DIRECTION_V_UP;
    }
    else
    {
        v_direction = DIRECTION_V_DOWN;
    }
    /* Read horizontal actuation button */
    if (Read_Horizontal_Button() == BUTTON_PRESSED)
    {
        Set_Horizontal_Movement(h_direction);
    }
    else
    {
        Set_Horizontal_Movement(DIRECTION_H_IDLE);
    }
    /* Read vertical actuation button */
    if (Read_Vertical_Button() == BUTTON_PRESSED)
    {
        Set_Vertical_Movement(v_direction);
    }
    else
    {
        Set_Vertical_Movement(DIRECTION_V_IDLE);
    }
}

/**
 * @brief Set horizontal movement direction.
 *
 * @param direction Direction enum for horizontal movement.
 */
void Set_Horizontal_Movement(Horizontal_Direction_t direction)
{

    PWM_Duty_Cycle_t cmd;
    cmd.channel = HORIZONTAL_SERVO_PWM;
    switch (direction)
    {
    case DIRECTION_H_CLOCKWISE:
        cmd.duty_cycle = 80;
        cmd.direction = DIRECTION_CLOCKWISE;
        break;
    case DIRECTION_H_COUNTERCLOCKWISE:
        cmd.duty_cycle = 80;
        cmd.direction = DIRECTION_COUNTERCLOCKWISE;
        break;
    case DIRECTION_H_IDLE:
    default:
        cmd.duty_cycle = 0;
        cmd.direction = DIRECTION_IDLE;
        break;
    }
    xQueueSend(PWM_Queue, &cmd, 0);
}

/**
 * @brief Set vertical movement direction.
 *
 * @param direction Direction enum for vertical movement.
 */
void Set_Vertical_Movement(Vertical_Direction_t direction)
{
    PWM_Duty_Cycle_t cmd;
    cmd.channel = VERTICAL_SERVO_PWM;
    switch (direction)
    {
    case DIRECTION_V_UP:
        cmd.duty_cycle = 80;
        cmd.direction = DIRECTION_CLOCKWISE;
        break;
    case DIRECTION_V_DOWN:
        cmd.duty_cycle = 80;
        cmd.direction = DIRECTION_COUNTERCLOCKWISE;
        break;
    case DIRECTION_V_IDLE:
    default:
        cmd.duty_cycle = 0;
        cmd.direction = DIRECTION_IDLE;
        break;
    }
    xQueueSend(PWM_Queue, &cmd, 0);
}