/*
 * UART_Driver.c
 *
 *  Created on: Oct. 21, 2022
 *      Author: Andre Hendricks / Dr. JF Bousquet
 */


#include <string.h>

#include "main.h"
#include "USART_Driver.h"


//Required FreeRTOS header files
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#define MAX_RX_BUFFER_LENGTH   40

uint8_t rx_buffer_extern[MAX_RX_BUFFER_LENGTH];
uint8_t rx_buffer_hostPC[MAX_RX_BUFFER_LENGTH];

QueueHandle_t Queue_extern_UART;
QueueHandle_t Queue_hostPC_UART;

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;

/******************************************************************************
This triggers a read and the buffer will be filled asynchronously.
******************************************************************************/
void request_hostPC_read(void)
{
	HAL_UART_Receive_IT (&huart2, rx_buffer_hostPC, 1); // Read 1 byte in interrupt mode
}

/******************************************************************************
Configures the external USART.
******************************************************************************/
void configure_usart_hostPC(void)
{
	//Start interrupt for Host PC UART
	request_hostPC_read();

	// a queue will be filled by the Host PC UART
	Queue_hostPC_UART = xQueueCreate(80, sizeof(uint8_t));
}

/******************************************************************************
******************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	BaseType_t xStatus = pdFAIL; // status from queue operation

	//Toggle onboard LED
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

	if(huart == &huart2){//Handle Host PC RX UART
		// fill the queue one character at a time
		xStatus = xQueueSendToBackFromISR(Queue_hostPC_UART, rx_buffer_hostPC, 0);

		//Request UART Interrupt Rx
		request_hostPC_read();
    }
	

	if(xStatus == pdPASS){
		//Toggle onboard LED
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	}

}
