/**
 * @file Command_Dispatch.c
 *
 * @brief Task for handling PC commands and dispatching them to appropriate modules.
 */

/* Module Header */
#include "Command_Dispatch.h"

/* Standard Libraries */
#include <string.h>

/* User Defined Libraries */
#include "user_main.h"
#include "L2/Comm_Datalink.h"

extern QueueHandle_t Command_Queue;

static void change_mode_handler(char arguments[6][16], uint8_t arg_count);

/* Command Entry Structure */
typedef struct COMMAND_ENTRY
{
    char command_string[16];
    void (*handler_function)(char arguments[6][16], uint8_t arg_count);
} Command_Entry_t;

/* Command Table */
Command_Entry_t Command_Table[] = {
    {"change_mode", change_mode_handler},
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
    }
    else if (strcmp(arguments[0], "manual") == 0)
    {
        print_str("Changing to MANUAL mode.\r\n");
    }
}