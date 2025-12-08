/**
 * @file Control_Loop.c
 *
 * @brief Implements closed-loop control for the automated warehouse crane.
 * Takes sensor inputs and adjusts motor outputs to maintain desired positions.
 * Uses PID control algorithms for precise movement.
 * Clamps motor commands to required safety limits.
 */

/* Module Header */
#include "L3/Control_Loop.h"

/* Standard Libraries */
#include <math.h>
#include <stdio.h>

/* User Libraries */
#include "user_main.h"
#include "L1/PWM_Driver.h"

#define PWM_MAX 40.0f  /* Max pulse width adjustment for PWM (in microseconds) */
#define PWM_MIN -30.0f /* Min pulse width adjustment for PWM (in microseconds) */
#define SETPOINT_MIN_MM 30.0f
#define SETPOINT_MAX_MM 130.0f

#define PID_ANTI_WINDUP_LIMIT 150.0f
#define DEADZONE_MM 2.0f

#define GRAVITY_COMPENSATION 0.7f

#define ULTRASONIC_SAMPLE_RATE_MS 30
#define STARTUP_SETPOINT_MM 100

extern QueueHandle_t Filtered_Ultrasonic_Queue;
extern QueueHandle_t PWM_Queue;
QueueHandle_t Motor_Setpoint_Queue;

static int32_t vertical_position_setpoint_mm = STARTUP_SETPOINT_MM;

typedef struct
{
    float Kp;
    float Ki;
    float Kd;
    float previous_error;
    float integral;
} PID_Controller_t;

static float PID_Compute(PID_Controller_t *pid, float error, float dT);

/**
 * @brief Task to update desired motor setpoint from queue.
 */
void Update_Motor_Setpoint_Task(void *pvParameters)
{
    uint32_t new_setpoint;
    while (1)
    {
        if (xQueueReceive(Motor_Setpoint_Queue, &new_setpoint, portMAX_DELAY) == pdTRUE)
        {
            vertical_position_setpoint_mm = new_setpoint;
        }
    }
    UNUSED(pvParameters);
}

/**
 * @brief Control loop task to maintain crane position using PID control.
 */
void Control_Loop_Task(void *pvParameters)
{
    PID_Controller_t vertical_pid = {
        .Kp = 15.0f, .Ki = 0.0f, .Kd = 0.0f, .previous_error = 0.0f, .integral = 0.0f}; /* Proportional only due to non-linearities */

    while (1)
    {
        int32_t current_position_mm;
        /* Read filtered ultrasonic distance */
        if (xQueueReceive(Filtered_Ultrasonic_Queue, &current_position_mm, pdMS_TO_TICKS(ULTRASONIC_SAMPLE_RATE_MS)) == pdTRUE)
        {
            float error = (float)(vertical_position_setpoint_mm - current_position_mm);
            char debug_string[64];
            sprintf(debug_string, "Error: %d mm\r\n", (int)(error));
            print_str(debug_string);
            float control_output = PID_Compute(&vertical_pid, error, ULTRASONIC_SAMPLE_RATE_MS / 1000.0f);
            control_output = -control_output; /* Invert control output for motor direction */

            /* Prepare PWM message */
            PWM_Duty_Cycle_t pwm_msg;
            pwm_msg.channel = VERTICAL_SERVO_PWM;
            pwm_msg.duty_cycle = (int16_t)control_output; /* Control output directly maps to pulse width adjustment */
            sprintf(debug_string, "Control Output: %d us\r\n", (int)control_output);
            print_str(debug_string);
            /* Send PWM command */
            xQueueSend(PWM_Queue, &pwm_msg, portMAX_DELAY);
        }
    }

    UNUSED(pvParameters);
}

/**
 * @brief Calculate PID control output.
 * @param pid Pointer to PID controller structure
 * @param setpoint Desired target value
 * @param measured Current measured value
 * @return Control output
 */
static float PID_Compute(PID_Controller_t *pid, float error, float dT)
{
    float proportional;
    float derivative;
    float output;

    /* Apply Deadzone */
    float effective_error = (fabsf(error) < DEADZONE_MM) ? 0.0f : error;

    /* Proportional Term */
    proportional = pid->Kp * effective_error;

    /* Accumulate Error term*/
    pid->integral += pid->Ki * effective_error * dT;

    /* Anti-windup clamp */
    pid->integral = fmaxf(-PID_ANTI_WINDUP_LIMIT, fminf(PID_ANTI_WINDUP_LIMIT, pid->integral));

    /* Calculate derivative term */
    derivative = pid->Kd * (error - pid->previous_error) / dT;

    /* Update Previous Error */
    pid->previous_error = error;

    /* Total Output */
    output = proportional + pid->integral + derivative;

    if (output < 0)
    {
        output *= GRAVITY_COMPENSATION; /* Compensate for gravity when moving up */
    }

    /* Clamp Total Output */
    output = fmaxf(PWM_MIN, fminf(PWM_MAX, output));

    return output;
}

void Set_Setpoint(uint32_t setpoint_mm)
{
    if (setpoint_mm < SETPOINT_MIN_MM)
    {
        setpoint_mm = SETPOINT_MIN_MM;
    }
    else if (setpoint_mm > SETPOINT_MAX_MM)
    {
        setpoint_mm = SETPOINT_MAX_MM;
    }
    xQueueSend(Motor_Setpoint_Queue, &setpoint_mm, portMAX_DELAY);
}