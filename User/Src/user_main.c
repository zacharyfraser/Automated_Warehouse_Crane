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

typedef struct Message{
    PC_Commands_t command;
    Mode_Types_t mode;
}Message_t;

// typedef struct Argument{
//     char type;
//     char 

// }Argument_t;

//Message_t global_state; // Global state variable 


/*
 * This task reads the queue of characters from the Host PC when available
 * It then sends the processed data to the Sensor Controller Task
 */
void HostPC_RX_Task(){
	
    configure_usart_hostPC();  // Configure Host PC UART and its queue
    //request_hostPC_read();  // Start the first read
    while(1)
    {
        vTaskDelay(1000/portTICK_RATE_MS);
    }
   
}

void Parser_Task()
{
	
    uint8_t counter = 0; // Index for received data
    char parse_storage[80]; // Storage for parsed data
    char value;
    Command_Queue = xQueueCreate(10, sizeof(Message_t)); // Create command queue
    
    Message_t System_Next_State; // Global state variable

    while(1) {
    if (xQueueReceive(Queue_hostPC_UART, &value, portMAX_DELAY) == pdTRUE) 
    {
            
        if (value == '\177') { // Handle backspace
            if (counter > 0) {
                counter--; // Move back the counter
            }
        }

        else if (value == ' ') { // If space is encountered, null-terminate, and store position of word
            parse_storage[counter] = '\0'; // Null-terminate the string here
            counter+=1; 
        }
        else if (value == '\r') { // Reached end of command, now start parsing 
            parse_storage[counter] = '\0'; 
                       
            if (strcmp(parse_storage, "change_command") == 0) { // 
                System_Next_State.command = CMD_CHANGE_MODE;
                
                if (strcmp(&parse_storage[15], "manual") == 0) {
                    System_Next_State.mode = MODE_MANU;
                    print_str("you are now in manual\r");
                }
                else if (strcmp(&parse_storage[15], "automatic") == 0) {
                    System_Next_State.mode = MODE_AUTO;
                    print_str("you are now in automatic\r");
                }
                else if (strcmp(&parse_storage[15], "calibrate") == 0) {
                    System_Next_State.mode = MODE_CALI;
                    print_str("you are now in calibrate\r");
                }
                else {
                    print_str("Valid Command but Invalid Mode\r");
                }
            }
            else {
                print_str("Invalid Command\r");
            }     
            
            xQueueSend(Command_Queue, &System_Next_State, portMAX_DELAY); // Send the new system state to the Command Queue
            counter = 0; // Reset counter 
        }
        else {
            parse_storage[counter] = value; // Store received character
            counter+=1;
        }

    }
    else 
    {
        counter = 0; // Reset counter if no data received
    }
} 

}

void user_main(void)
{
    util_init();
    osKernelInitialize(); /* Call init function for freertos objects (in cmsis_os2.c) */
    MX_FREERTOS_Init();   /* CubeMX Generated FreeRTOS objects are initialized here */

    xTaskCreate(HostPC_RX_Task,"Host Rx Task", configMINIMAL_STACK_SIZE + 100, NULL,
tskIDLE_PRIORITY + 2, NULL);
xTaskCreate(Parser_Task,"Parser Task", configMINIMAL_STACK_SIZE + 100, NULL,
tskIDLE_PRIORITY + 2, NULL);
    osKernelStart();      /* Start scheduler */
}