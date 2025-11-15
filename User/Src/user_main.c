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
#include "L1/PWM_Driver.h"

void MX_FREERTOS_Init(void);

QueueHandle_t PWM_Queue;

/* TODO - Move HostPC_RX_Task() to L1/USART_Driver.c */
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
char main_string[50];
uint32_t main_counter = 0;
char value;

/**
 * @brief This task reads the queue of characters from the Host PC when available
 * It then sends the processed data to the Sensor Controller Task
 */
void HostPC_RX_Task(void *pvParameters)
{

    configure_usart_hostPC(); // Configure Host PC UART and its queue
    // request_hostPC_read();  // Start the first read
    while (1)
    {
        // Wait indefinitely for data to arrive in the queue
        if (xQueueReceive(Queue_hostPC_UART, &value, portMAX_DELAY) == pdTRUE)
        {
            // Process the received character (for demonstration, we just print it)
            sprintf(main_string, "Received from Host PC: %c\r\n", value);
            print_str(main_string);
        }
    }
    UNUSED(pvParameters);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/**
 * @brief This task is for debugging purposes
 * The implementation will be changed as needed
 */
void Debug_Task(void *pvParameters)
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
    /* Debug Task */
    xTaskCreate(Debug_Task, "Debug_Task", configMINIMAL_STACK_SIZE + 100, NULL,
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