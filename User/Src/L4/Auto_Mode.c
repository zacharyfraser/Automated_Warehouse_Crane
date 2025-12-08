/**
 * @file Auto_Mode.c
 *
 * @brief Implements automatic mode operations for the warehouse crane.
 * Contains state machine logic for moving between predefined positions.
 */

/* Module Header */
#include "L4/Auto_Mode.h"
#include "L3/Control_Loop.h"
#include "L1/PWM_Driver.h"

/* Standard Libraries */

/* User Libraries */
#include "user_main.h"

#define HOME_POSITION_MM (60)
#define LOWER_LATCH_POSITION_MM (70)
#define UPPER_SHELF_POSITION_MM (130)
#define UPPER_DROPOFF_POSITION_MM (120)

#define COUNTERCLOCKWISE_ROTATE_TIME_MS (2000)
#define CLOCKWISE_ROTATE_TIME_MS (2000)

typedef enum Auto_States
{
    STATE_AUTO_IDLE = 0,
    STATE_AUTO_START,
    STATE_AUTO_MOVE_VERTICAL_TO_HOME,
    STATE_AUTO_MOVE_HORIZONTAL_TO_LOWER_PICKUP,
    STATE_AUTO_MOVE_VERTICAL_TO_LOWER_LATCH,
    STATE_AUTO_MOVE_HORIZONTAL_TO_CARRY_POSITION,
    STATE_AUTO_MOVE_VERTICAL_TO_UPPER_SHELF,
    STATE_AUTO_MOVE_HORIZONTAL_TO_UPPER_DROPOFF,
    STATE_AUTO_MOVE_VERTICAL_TO_UPPER_DROPOFF,
    STATE_AUTO_MOVE_HORIZONTAL_TO_HOME_FROM_UPPER,
    STATE_AUTO_MOVE_VERTICAL_TO_HOME_FROM_UPPER
} Auto_States_t;

extern EventGroupHandle_t Motor_Event_Group;
extern QueueHandle_t PWM_Queue;
static Auto_States_t auto_state;

/**
 * @brief Reset and initialize automatic mode.
 */
void Initialize_Auto_Mode(void)
{
    auto_state = STATE_AUTO_START;
}

/**
 * @brief Run the automatic mode state machine.
 *
 * Note - Due to limited horizontal arm feedback, the arm is
 * assumed to be in the center when entering automatic mode.
 */
void Run_Auto_Mode(void)
{
    PWM_Duty_Cycle_t cmd;
    cmd.channel = HORIZONTAL_SERVO_PWM;
    switch (auto_state)
    {
    case STATE_AUTO_START:
        print_str("Entering automatic mode\r\n");
        /* Enable PID */
        Toggle_PID_Control(true);
        /* Set Setpoint to home position */
        Set_Setpoint(HOME_POSITION_MM);
        /* Clear Wait for Motor Event */
        xEventGroupClearBits(Motor_Event_Group, MOTOR_EVENT_BIT);
        auto_state = STATE_AUTO_MOVE_VERTICAL_TO_HOME;
        break;
    case STATE_AUTO_MOVE_VERTICAL_TO_HOME:
        print_str("Moving vertical to home position\r\n");
        /* Wait for vertical movement to reach home position */
        if (xEventGroupWaitBits(Motor_Event_Group, MOTOR_EVENT_BIT, pdTRUE, pdTRUE, portMAX_DELAY) & MOTOR_EVENT_BIT)
        {
            auto_state = STATE_AUTO_MOVE_HORIZONTAL_TO_LOWER_PICKUP;
        }
        break;
    case STATE_AUTO_MOVE_HORIZONTAL_TO_LOWER_PICKUP:
        print_str("Moving horizontal to lower pickup position\r\n");
        /* Move Horizontal Arm Counterclockwise */
        cmd.direction = DIRECTION_COUNTERCLOCKWISE;
        cmd.duty_cycle = 75;
        xQueueSend(PWM_Queue, &cmd, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(COUNTERCLOCKWISE_ROTATE_TIME_MS));
        cmd.direction = DIRECTION_IDLE;
        cmd.duty_cycle = 0;
        xQueueSend(PWM_Queue, &cmd, portMAX_DELAY);
        Set_Setpoint(LOWER_LATCH_POSITION_MM);
        /* Clear Wait for Motor Event */
        xEventGroupClearBits(Motor_Event_Group, MOTOR_EVENT_BIT);
        auto_state = STATE_AUTO_MOVE_VERTICAL_TO_LOWER_LATCH;
        break;
    case STATE_AUTO_MOVE_VERTICAL_TO_LOWER_LATCH:
        print_str("Moving vertical to lower latch position\r\n");
        /* Wait for vertical movement to reach lower latch position */
        if (xEventGroupWaitBits(Motor_Event_Group, MOTOR_EVENT_BIT, pdTRUE, pdTRUE, portMAX_DELAY) & MOTOR_EVENT_BIT)
        {
            auto_state = STATE_AUTO_MOVE_HORIZONTAL_TO_CARRY_POSITION;
        }
        break;
    case STATE_AUTO_MOVE_HORIZONTAL_TO_CARRY_POSITION:
        print_str("Moving horizontal to carry position\r\n");
        /* Move Horizontal Arm Clockwise */
        cmd.direction = DIRECTION_CLOCKWISE;
        cmd.duty_cycle = 75;
        xQueueSend(PWM_Queue, &cmd, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(CLOCKWISE_ROTATE_TIME_MS));
        cmd.direction = DIRECTION_IDLE;
        cmd.duty_cycle = 0;
        xQueueSend(PWM_Queue, &cmd, portMAX_DELAY);
        Set_Setpoint(UPPER_SHELF_POSITION_MM); /* Move to upper shelf position */
        /* Clear Wait for Motor Event */
        xEventGroupClearBits(Motor_Event_Group, MOTOR_EVENT_BIT);
        auto_state = STATE_AUTO_MOVE_VERTICAL_TO_UPPER_SHELF;
        break;
    case STATE_AUTO_MOVE_VERTICAL_TO_UPPER_SHELF:
        print_str("Moving vertical to upper shelf position\r\n");
        /* Wait for vertical movement to reach upper shelf position */
        if (xEventGroupWaitBits(Motor_Event_Group, MOTOR_EVENT_BIT, pdTRUE, pdTRUE, portMAX_DELAY) & MOTOR_EVENT_BIT)
        {
            auto_state = STATE_AUTO_MOVE_HORIZONTAL_TO_UPPER_DROPOFF;
        }
        break;
    case STATE_AUTO_MOVE_HORIZONTAL_TO_UPPER_DROPOFF:
        print_str("Moving horizontal to upper dropoff position\r\n");
        /* Move Horizontal Arm Clockwise */
        cmd.direction = DIRECTION_CLOCKWISE;
        cmd.duty_cycle = 75;
        xQueueSend(PWM_Queue, &cmd, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(CLOCKWISE_ROTATE_TIME_MS));
        cmd.direction = DIRECTION_IDLE;
        cmd.duty_cycle = 0;
        xQueueSend(PWM_Queue, &cmd, portMAX_DELAY);
        Set_Setpoint(UPPER_DROPOFF_POSITION_MM); /* Move to upper dropoff position */
        /* Clear Wait for Motor Event */
        xEventGroupClearBits(Motor_Event_Group, MOTOR_EVENT_BIT);
        auto_state = STATE_AUTO_MOVE_VERTICAL_TO_UPPER_DROPOFF;
        break;
    case STATE_AUTO_MOVE_VERTICAL_TO_UPPER_DROPOFF:
        print_str("Moving vertical to upper dropoff position\r\n");
        /* Wait for vertical movement to reach upper dropoff position */
        if (xEventGroupWaitBits(Motor_Event_Group, MOTOR_EVENT_BIT, pdTRUE, pdTRUE, portMAX_DELAY) & MOTOR_EVENT_BIT)
        {
            auto_state = STATE_AUTO_MOVE_HORIZONTAL_TO_HOME_FROM_UPPER;
        }
        break;
    case STATE_AUTO_MOVE_HORIZONTAL_TO_HOME_FROM_UPPER:
        print_str("Moving horizontal to home from upper position\r\n");
        /* Move Horizontal Arm Counterclockwise */
        cmd.direction = DIRECTION_COUNTERCLOCKWISE;
        cmd.duty_cycle = 75;
        xQueueSend(PWM_Queue, &cmd, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(COUNTERCLOCKWISE_ROTATE_TIME_MS));
        cmd.direction = DIRECTION_IDLE;
        cmd.duty_cycle = 0;
        xQueueSend(PWM_Queue, &cmd, portMAX_DELAY);
        Set_Setpoint(HOME_POSITION_MM);
        /* Clear Wait for Motor Event */
        xEventGroupClearBits(Motor_Event_Group, MOTOR_EVENT_BIT);
        auto_state = STATE_AUTO_MOVE_VERTICAL_TO_HOME_FROM_UPPER;
        break;
    case STATE_AUTO_MOVE_VERTICAL_TO_HOME_FROM_UPPER:
        print_str("Moving vertical to home from upper position\r\n");
        /* Wait for vertical movement to reach home position */
        if (xEventGroupWaitBits(Motor_Event_Group, MOTOR_EVENT_BIT, pdTRUE, pdTRUE, portMAX_DELAY) & MOTOR_EVENT_BIT)
        {
            print_str("Automatic mode sequence complete\r\n");
            /* Disable PID */
            Toggle_PID_Control(false);
            auto_state = STATE_AUTO_IDLE;
        }
        break;
    case STATE_AUTO_IDLE:
    default:
        /* Do nothing, remain idle */
        break;
    }
}