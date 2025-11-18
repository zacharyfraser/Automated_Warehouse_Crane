/**
 * @file    user_main.c
 *
 * @brief   Source file for user_main.c
 *
 * Using FreeRTOS-specific calls instead of CMSIS RTOS calls
 *
 * Created on: November 11, 2025
 */

/* Module Header */
#include "user_main.h"

/* User Libraries */
#include "Debug.h"
#include "L1/USART_Driver.h"
#include "L1/PWM_Driver.h"
#include "L1/Ultrasonic_Driver.h"
#include "L2/Comm_Datalink.h"
#include "L3/Command_Dispatch.h"

extern QueueHandle_t PWM_Queue;
extern QueueHandle_t Command_Queue;
extern QueueHandle_t Queue_hostPC_UART;
extern QueueHandle_t Ultrasonic_Raw_Queue;

/* Local function prototypes */
void create_queues(void);
void create_initial_tasks(void);

/**
 * @brief User main function to initialize and start the RTOS kernel.
 */
void user_main(void)
{
    /* Initialize UART Print functions */
    util_init();

    /* Create User-made FreeRTOS objects */
    create_queues();
    create_initial_tasks();

    vTaskStartScheduler();
}

/**
 * @brief Create all inter-task queues
 *
 * This function is called from user_main() to create the necessary queues before starting the scheduler.
 */
void create_queues(void)
{
    /* Update PWM pulse widths */
    PWM_Queue = xQueueCreate(10, sizeof(PWM_Duty_Cycle_t));
    /* Commands received from Host PC */
    Command_Queue = xQueueCreate(10, sizeof(Message_t));
    /* Queue for Host PC UART characters*/
    Queue_hostPC_UART = xQueueCreate(80, sizeof(uint8_t));
    /* Queue for Ultrasonic sensor readings */
    Ultrasonic_Raw_Queue = xQueueCreate(10, sizeof(uint32_t));
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
    /* Start Ultrasonic Sensor Read Task */
    xTaskCreate(Ultrasonic_Read_Task, "Ultrasonic_Read_Task", configMINIMAL_STACK_SIZE + 100, NULL,
                tskIDLE_PRIORITY + 2, NULL);
    /* Start UART string tokenizer */
    xTaskCreate(Tokenize_Task, "Tokenize Task", configMINIMAL_STACK_SIZE + 100, NULL,
                tskIDLE_PRIORITY + 2, NULL);
    /* Command Dispatch Task */
    xTaskCreate(Command_Dispatch_Task, "Command Dispatch Task", configMINIMAL_STACK_SIZE + 200, NULL,
                tskIDLE_PRIORITY + 2, NULL);
    /* Debug Tasks */
    xTaskCreate(Debug_Task1, "Debug_Task1", configMINIMAL_STACK_SIZE + 100, NULL,
                tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(Debug_Task2, "Debug_Task2", configMINIMAL_STACK_SIZE + 100, NULL,
                tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(Debug_Task3, "Debug_Task3", configMINIMAL_STACK_SIZE + 100, NULL,
                tskIDLE_PRIORITY + 1, NULL);
}