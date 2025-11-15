#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "main.h"
#include "util.h"
#include <stdio.h>
#include "queue.h"
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
void HostPC_RX_Task()
{

    configure_usart_hostPC(); // Configure Host PC UART and its queue
    while (1)
    {
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void Tokenize_Task()
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
}

void Debug_Task()
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
}

void user_main(void)
{
    util_init();
    osKernelInitialize(); /* Call init function for freertos objects (in cmsis_os2.c) */
    MX_FREERTOS_Init();   /* CubeMX Generated FreeRTOS objects are initialized here */

    xTaskCreate(HostPC_RX_Task, "Host Rx Task", configMINIMAL_STACK_SIZE + 100, NULL,
                tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(Tokenize_Task, "Tokenize Task", configMINIMAL_STACK_SIZE + 100, NULL,
                tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(Debug_Task, "Debug Task", configMINIMAL_STACK_SIZE + 100, NULL,
                tskIDLE_PRIORITY + 1, NULL);
    osKernelStart(); /* Start scheduler */
}