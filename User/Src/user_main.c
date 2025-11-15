/**
 * @file    user_main.c
 *
 * @brief   Source file for user_main.c
 *
 * Created on: November 11, 2025
 */
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "main.h"
#include "util.h"
#include <stdio.h>
#include "queue.h"

/* User Includes */
#include "L1/USART_Driver.h"
#include "user_main.h"
#include "string.h"

void MX_FREERTOS_Init(void);

/**
 * @brief User main function to initialize and start the RTOS kernel.
 */

char main_string[256];
// uint32_t main_counter = 0;
// char value;
QueueHandle_t Command_Queue;

typedef struct Message
{
    char command[32];
    char arguments[6][16];
    uint8_t arg_count;
} Message_t;

/*
 * This task reads the queue of characters from the Host PC when available
 * It then sends the processed data to the Sensor Controller Task
 */
void HostPC_RX_Task(void *pvParameters)
{

    configure_usart_hostPC(); // Configure Host PC UART and its queue
    while (1)
    {
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
    UNUSED(pvParameters);
}

void Tokenize_Task(void *pvParameters)
{
    uint8_t counter = 0; // Index for received data
    uint8_t argument_counter = 0;
    char parse_storage[32]; // Storage for parsed data
    char value;
    Command_Queue = xQueueCreate(10, sizeof(Message_t)); // Create command queue

    Message_t System_Next_State;

    while (1)
    {
        if (xQueueReceive(Queue_hostPC_UART, &value, portMAX_DELAY) == pdTRUE)
        {

            switch (value)
            {

            case '\177':
                if (counter > 0)
                {
                    counter--; // Move back the counter
                }
                break;

            case ' ':
                parse_storage[counter] = '\0'; // Null-terminate
                if (argument_counter == 0)
                { // user command
                    strcpy(System_Next_State.command, parse_storage);
                }
                else
                {
                    strcpy(System_Next_State.arguments[argument_counter - 1], parse_storage);
                }
                argument_counter += 1; // increment argument counter
                counter = 0;           // reset counter and overwrite parse storage
                break;

            case '\r':                         // end of user statement
                parse_storage[counter] = '\0'; // Null-terminate the string here
                if (argument_counter == 0)
                { // user command
                    strcpy(System_Next_State.command, parse_storage);
                }
                else
                {
                    strcpy(System_Next_State.arguments[argument_counter - 1], parse_storage);
                }
                System_Next_State.arg_count = argument_counter;
                xQueueSend(Command_Queue, &System_Next_State, portMAX_DELAY);
                argument_counter = 0; // Send the new system state to the Command Queue
                counter = 0;
                break;

            default:
                parse_storage[counter] = value; // Store received character
                counter += 1;
            }
        }
        else
        {
            counter = 0; // Reset counter if no data received
        }
    }
    UNUSED(pvParameters);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/**
 * @brief This task is for debugging purposes
 * The implementation will be changed as needed
 */
void Debug_Task1(void *pvParameters)
{
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
/**
 * @brief This task is for debugging purposes
 * The implementation will be changed as needed
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

/**
 * @brief Create all inter-task queues
 *
 * This function is called from user_main() to create the necessary queues before starting the scheduler.
 */
void create_queues(void)
{
    /* Create PWM Queue */
    PWM_Queue = xQueueCreate(10, sizeof(PWM_Duty_Cycle_t));
}

/**
 * @brief Create all initial tasks
 *
 * This function is called from user_main() to create the initial tasks before starting the scheduler.
 */
void create_initial_tasks(void)
{
    /* RX Task to receive communication with Host PC */
    xTaskCreate(HostPC_RX_Task, "RX_Task", configMINIMAL_STACK_SIZE + 100, NULL,
                tskIDLE_PRIORITY + 2, NULL);
    /* Start PWM Timers for servo control */
    xTaskCreate(PWM_Timer_Task, "PWM_Timer_Task", configMINIMAL_STACK_SIZE + 100, NULL,
                tskIDLE_PRIORITY + 2, NULL);
   /* Start UART string tokenizer */
   xTaskCreate(Tokenize_Task, "Tokenize Task", configMINIMAL_STACK_SIZE + 100, NULL,
                tskIDLE_PRIORITY + 2, NULL);
    /* Debug Tasks */
    xTaskCreate(Debug_Task1, "Debug_Task1", configMINIMAL_STACK_SIZE + 100, NULL,
                tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(Debug_Task1, "Debug_Task2", configMINIMAL_STACK_SIZE + 100, NULL,
                tskIDLE_PRIORITY + 1, NULL);
}

/**
 * @brief User main function to initialize and start the RTOS kernel.
 */
void user_main(void)
{
    util_init();
    osKernelInitialize(); /* Call init function for freertos objects (in cmsis_os2.c) */
    MX_FREERTOS_Init();   /* CubeMX Generated FreeRTOS objects are initialized here */

    /* Create User-made FreeRTOS objects */
    create_queues();
    create_initial_tasks();

    osKernelStart(); /* Start scheduler */
}