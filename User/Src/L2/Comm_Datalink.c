/**
 * @file Comm_Datalink.c
 *
 * @brief Tokenizes incoming UART data from Host PC into commands and arguments.
 * TODO: Packages system data into strings for transmission back to Host PC.
 */

/* Module Header */
#include "L2/Comm_Datalink.h"

/* Standard Libraries */
#include <string.h>
#include <ctype.h>

/* User Libraries */
#include "user_main.h"
#include "L1/USART_Driver.h"

QueueHandle_t Command_Queue;
extern QueueHandle_t Queue_hostPC_UART;

/**
 * @brief Task to tokenize incoming UART data from Host PC into commands and arguments.
 */
void Tokenize_Task(void *pvParameters)
{
    uint8_t counter = 0; // Index for received data
    uint8_t argument_counter = 0;
    char parse_storage[32]; // Storage for parsed data
    char value;

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
                parse_storage[counter] = tolower(value); // Store received character as lowercase
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