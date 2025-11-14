#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "main.h"
#include "util.h"
#include <stdio.h>
#include "queue.h"
#include "L1/USART_Driver.h"

void MX_FREERTOS_Init(void);

/**
 * @brief User main function to initialize and start the RTOS kernel.
 */


char main_string[50];
uint32_t main_counter = 0;
char value;


/*
 * This task reads the queue of characters from the Host PC when available
 * It then sends the processed data to the Sensor Controller Task
 */
void HostPC_RX_Task(){
	
    configure_usart_hostPC();  // Configure Host PC UART and its queue
    //request_hostPC_read();  // Start the first read
    while(1)
    {
        //Wait indefinitely for data to arrive in the queue
        if (xQueueReceive(Queue_hostPC_UART, &value, portMAX_DELAY) == pdTRUE)
        {
            // Process the received character (for demonstration, we just print it)
            sprintf(main_string, "Received from Host PC: %c\r\n", value);
            print_str(main_string);
        }
        //vTaskDelay(1000/portTICK_RATE_MS);
    }
    //print_str("Queue contents:\r\n");

    // while (xQueueReceive(Queue_hostPC_UART, &value, 0) == pdTRUE)
    // {
    //     sprintf(main_string,"queue value: %c", value);
    //     print_str(main_string);

    // }
// while(1){
//  print_str("Main task loop executing\r\n");
//  sprintf(main_string,"Main task iteration: 0x%08lx\r\n", main_counter++);
//  print_str(main_string);
//  vTaskDelay(1000/portTICK_RATE_MS);
//  }
}


void user_main(void)
{
    util_init();
    osKernelInitialize(); /* Call init function for freertos objects (in cmsis_os2.c) */
    MX_FREERTOS_Init();   /* CubeMX Generated FreeRTOS objects are initialized here */

    xTaskCreate(HostPC_RX_Task,"Main Task", configMINIMAL_STACK_SIZE + 100, NULL,
tskIDLE_PRIORITY + 2, NULL);
    osKernelStart();      /* Start scheduler */
}