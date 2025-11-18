/**
 * @file Debug.c
 *
 * @brief Debug task implementations.
 */

/* Module Header */
#include "Debug.h"

/* Standard Libraries */
#include <stdio.h> /* For sprintf */

/* User Libraries */
#include "user_main.h"
#include "L2/Comm_Datalink.h"
#include "L1/PWM_Driver.h"

extern QueueHandle_t Command_Queue;
extern QueueHandle_t PWM_Queue;
extern QueueHandle_t Ultrasonic_Raw_Queue;

// #define DEBUG1
// #define DEBUG2
#define DEBUG3

#ifdef DEBUG1
/**
 * @brief Debug task to print received commands and arguments.  Will not work if higher priority task reading from Command_Queue.
 */
void Debug_Task1(void *pvParameters)
{
    char main_string[256];
    Message_t received_message;
    while (1)
    {
        xQueueReceive(Command_Queue, &received_message, portMAX_DELAY);
        for (uint8_t i = 0; i < received_message.arg_count + 1; i++)
        {
            if (i == 0)
            {
                sprintf(main_string, "Command: %s\r\n", received_message.command);
                print_str(main_string);
            }
            else
            {
                sprintf(main_string, "Arg %d: %s\r\n", i, received_message.arguments[i - 1]);
                print_str(main_string);
            }
        }
    }
    UNUSED(pvParameters);
}
#else
void Debug_Task1(void *pvParameters)
{
    while (1)
    {
        vTaskDelete(NULL);
    }
    UNUSED(pvParameters);
}
#endif

#ifdef DEBUG2
/**
 * @brief Debug task to ramp PWM duty cycle for testing
 */
void Debug_Task2(void *pvParameters)
{
    int pulse_width = 0;
    int inc = 1;
    PWM_Duty_Cycle_t pwm_msg;
    while (1)
    {
        /* Ramp PWM duty cycle up and down */
        pwm_msg.channel = VERTICAL_SERVO_PWM;
        pwm_msg.pulse_width_percent = pulse_width;
        xQueueSend(PWM_Queue, &pwm_msg, portMAX_DELAY);

        pulse_width += inc;
        if (pulse_width > 99 || pulse_width < 1)
        {
            inc *= -1;
        }

        vTaskDelay(pdMS_TO_TICKS(10)); /* Delay for 100 milliseconds */
    }
    UNUSED(pvParameters);
}
#else
void Debug_Task2(void *pvParameters)
{
    while (1)
    {
        vTaskDelete(NULL);
    }
    UNUSED(pvParameters);
}
#endif

#ifdef DEBUG3

/**
 * @brief Debug task to read ultrasonic sensor distances and print them.
 */
void Debug_Task3(void *pvParameters)
{
    uint32_t distance_mm;
    char debug_string[64];

    while (1)
    {
        /* Read distance from Ultrasonic Queue */
        if (xQueueReceive(Ultrasonic_Raw_Queue, &distance_mm, portMAX_DELAY) == pdTRUE)
        {
            /* Print the distance */
            sprintf(debug_string, "Ultrasonic Distance: %lu mm\r\n", (unsigned long)distance_mm);
            print_str(debug_string);
        }
    }

    UNUSED(pvParameters);
}
#else
void Debug_Task3(void *pvParameters)
{
    while (1)
    {
        vTaskDelete(NULL);
    }
    UNUSED(pvParameters);
}
#endif