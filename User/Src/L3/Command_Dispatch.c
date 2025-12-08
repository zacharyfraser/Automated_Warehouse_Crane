/**
 * @file Command_Dispatch.c
 *
 * @brief Task for handling PC commands and dispatching them to appropriate modules.
 */

/* Module Header */
#include "Command_Dispatch.h"

/* Standard Libraries */
#include <string.h>
#include <stdlib.h>

/* User Defined Libraries */
#include "user_main.h"
#include "L2/Comm_Datalink.h"
#include "L3/Control_Loop.h"
#include "L5/Mode_Control.h"
#include "L1/PWM_Driver.h"

extern QueueHandle_t Command_Queue;
extern QueueHandle_t PWM_Queue;

static void change_mode_handler(char arguments[6][16], uint8_t arg_count);
static void set_setpoint_handler(char arguments[6][16], uint8_t arg_count);
static void set_horizontal_speed_handler(char arguments[6][16], uint8_t arg_count);
static void set_vertical_speed_handler(char arguments[6][16], uint8_t arg_count);
static void toggle_pid_control_handler(char arguments[6][16], uint8_t arg_count);
static void set_pid_proportional_gain_handler(char arguments[6][16], uint8_t arg_count);
static void set_pid_integral_gain_handler(char arguments[6][16], uint8_t arg_count);
static void set_pid_derivative_gain_handler(char arguments[6][16], uint8_t arg_count);
static void get_pid_gains_handler(char arguments[6][16], uint8_t arg_count);

/* Command Entry Structure */
typedef struct COMMAND_ENTRY
{
    char command_string[16];
    void (*handler_function)(char arguments[6][16], uint8_t arg_count);
} Command_Entry_t;

/* Command Table */
Command_Entry_t Command_Table[] = {
    {"chmd", change_mode_handler},
    {"spt", set_setpoint_handler},
    {"hvel", set_horizontal_speed_handler},
    {"vvel", set_vertical_speed_handler},
    {"pid", toggle_pid_control_handler},
    {"pidp", set_pid_proportional_gain_handler},
    {"pidi", set_pid_integral_gain_handler},
    {"pidd", set_pid_derivative_gain_handler},
    {"gpid", get_pid_gains_handler},
};

/**
 * @brief Task to handle PC commands and dispatch them to appropriate modules.
 */
void Command_Dispatch_Task(void *pvParameters)
{
    Message_t Received_Command;

    while (1)
    {
        if (xQueueReceive(Command_Queue, &Received_Command, portMAX_DELAY) == pdTRUE)
        {
            /* Dispatch command to appropriate handler */
            for (size_t i = 0; i < sizeof(Command_Table) / sizeof(Command_Entry_t); i++)
            {
                if (strcmp(Received_Command.command, Command_Table[i].command_string) == 0)
                {
                    /* Call the handler function and pass arguments */
                    Command_Table[i].handler_function(Received_Command.arguments, Received_Command.arg_count);
                    break;
                }
            }
        }
    }

    UNUSED(pvParameters);
}

/**
 * @brief Handler for the "set_setpoint" command.
 *
 * Sets the vertical position setpoint for the motor control loop.
 * @param arguments Array of argument strings.
 * @param arg_count Number of arguments provided.
 */
static void set_setpoint_handler(char arguments[6][16], uint8_t arg_count)
{
    if (arg_count < 1)
    {
        return;
    }

    int32_t new_setpoint = atoi(arguments[0]);
    Set_Setpoint(new_setpoint);
}

/**
 * @brief Handler for the "change_mode" command.
 *
 * Changes the operational mode of the system based on the provided argument.
 *
 * @param arguments Array of argument strings.
 * @param arg_count Number of arguments provided.
 */
static void change_mode_handler(char arguments[6][16], uint8_t arg_count)
{
    if (arg_count < 1)
    {
        return;
    }

    if (strcmp(arguments[0], "auto") == 0)
    {
        print_str("Changing to AUTO mode.\r\n");
        Transition_Mode(MODE_AUTOMATIC);
    }
    else if (strcmp(arguments[0], "manual") == 0)
    {
        print_str("Changing to MANUAL mode.\r\n");
        Transition_Mode(MODE_MANUAL);
    }
    else if (strcmp(arguments[0], "calibrate") == 0)
    {
        print_str("Changing to CALIBRATION mode.\r\n");
        Transition_Mode(MODE_CALIBRATION);
    }
}

/**
 * @brief Handler for the "hv" command.
 *
 * Sets the horizontal servo speed.
 *
 * @param arguments Array of argument strings.
 * @param arg_count Number of arguments provided.
 */
static void set_horizontal_speed_handler(char arguments[6][16], uint8_t arg_count)
{
    if (arg_count < 1)
    {
        return;
    }

    int32_t new_speed = atoi(arguments[0]);

    PWM_Duty_Cycle_t pwm_msg;
    pwm_msg.channel = HORIZONTAL_SERVO_PWM;
    if (new_speed > 0)
    {
        pwm_msg.direction = DIRECTION_CLOCKWISE;
    }
    else if (new_speed < 0)
    {
        pwm_msg.direction = DIRECTION_COUNTERCLOCKWISE;
        new_speed = -new_speed; /* Make speed positive for duty cycle */
    }
    else
    {
        pwm_msg.direction = DIRECTION_IDLE;
    }
    pwm_msg.duty_cycle = (int16_t)new_speed;
    xQueueSend(PWM_Queue, &pwm_msg, portMAX_DELAY);
}

/**
 * @brief Handler for the "vvel" command.
 *
 * Sets the vertical servo speed.
 *
 * @param arguments Array of argument strings.
 * @param arg_count Number of arguments provided.
 */
static void set_vertical_speed_handler(char arguments[6][16], uint8_t arg_count)
{
    if (arg_count < 1)
    {
        return;
    }

    int32_t new_speed = atoi(arguments[0]);

    PWM_Duty_Cycle_t pwm_msg;
    pwm_msg.channel = VERTICAL_SERVO_PWM;
    if (new_speed > 0)
    {
        pwm_msg.direction = DIRECTION_CLOCKWISE;
    }
    else if (new_speed < 0)
    {
        pwm_msg.direction = DIRECTION_COUNTERCLOCKWISE;
        new_speed = -new_speed; /* Make speed positive for duty cycle */
    }
    else
    {
        pwm_msg.direction = DIRECTION_IDLE;
    }
    pwm_msg.duty_cycle = (int16_t)new_speed;
    xQueueSend(PWM_Queue, &pwm_msg, portMAX_DELAY);
}

/**
 * @brief Handler for the "pid" command.
 *
 * Enables or disables the PID control loop.
 *
 * @param arguments Array of argument strings.
 * @param arg_count Number of arguments provided.
 */
static void toggle_pid_control_handler(char arguments[6][16], uint8_t arg_count)
{
    if (arg_count < 1)
    {
        return;
    }

    if (strcmp(arguments[0], "on") == 0)
    {
        Toggle_PID_Control(true);
        print_str("PID control enabled.\r\n");
    }
    else if (strcmp(arguments[0], "off") == 0)
    {
        Toggle_PID_Control(false);
        print_str("PID control disabled.\r\n");
    }
}

/**
 * @brief Handler for the "pidp" command.
 *
 * Sets the proportional gain for the PID controller.
 *
 * @param arguments Array of argument strings.
 * @param arg_count Number of arguments provided.
 */
static void set_pid_proportional_gain_handler(char arguments[6][16], uint8_t arg_count)
{
    if (arg_count < 1)
    {
        return;
    }

    float Kp = atof(arguments[0]);
    Set_Proportional_Gain(Kp);
    print_str("PID proportional gain updated.\r\n");
}

/**
 * @brief Handler for the "pidi" command.
 *
 * Sets the integral gain for the PID controller.
 *
 * @param arguments Array of argument strings.
 * @param arg_count Number of arguments provided.
 */
static void set_pid_integral_gain_handler(char arguments[6][16], uint8_t arg_count)
{
    if (arg_count < 1)
    {
        return;
    }

    float Ki = atof(arguments[0]);
    Set_Integral_Gain(Ki);
    print_str("PID integral gain updated.\r\n");
}

/**
 * @brief Handler for the "pidd" command.
 *
 * Sets the derivative gain for the PID controller.
 *
 * @param arguments Array of argument strings.
 * @param arg_count Number of arguments provided.
 */
static void set_pid_derivative_gain_handler(char arguments[6][16], uint8_t arg_count)
{
    if (arg_count < 1)
    {
        return;
    }

    float Kd = atof(arguments[0]);
    Set_Derivative_Gain(Kd);
    print_str("PID derivative gain updated.\r\n");
}

/**
 * @brief Handler for the "gpid" command.
 *
 * Retrieves and prints the current PID gains.
 *
 * @param arguments Array of argument strings.
 * @param arg_count Number of arguments provided.
 */
static void get_pid_gains_handler(char arguments[6][16], uint8_t arg_count)
{
    Print_PID_Gains();
}