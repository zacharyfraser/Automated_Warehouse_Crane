/*
 * UART_Driver.c
 *
 *  Created on: Oct. 21, 2022
 *      Author: Andre Hendricks / Dr. JF Bousquet
 */

/* Module Header */
#include "L1/USART_Driver.h"

/* Standard Libraries*/
#include <string.h>

/* User Libraries */
#include "user_main.h"

#define MAX_RX_BUFFER_LENGTH 40

uint8_t rx_buffer_extern[MAX_RX_BUFFER_LENGTH];
uint8_t rx_buffer_hostPC[MAX_RX_BUFFER_LENGTH];

extern UART_HandleTypeDef huart2;
QueueHandle_t Queue_hostPC_UART;

/**
 * @brief Starts UART receive interrupt for Host PC UART
 */
void request_hostPC_read(void)
{
	HAL_UART_Receive_IT(&huart2, rx_buffer_hostPC, 1); /* Read 1 byte in interrupt mode */
}

/**
 * @brief UART RX Complete Callback
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	BaseType_t xStatus = pdFAIL; /* status from queue operation */

	/* Toggle onboard LED */
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

	if (huart == &huart2)
	{
		/* Fill the queue one character at a time */
		xStatus = xQueueSendToBackFromISR(Queue_hostPC_UART, rx_buffer_hostPC, 0);

		/* Request next character */
		request_hostPC_read();
	}

	if (xStatus == pdPASS)
	{
		/* Toggle onboard LED to indicate successful RX */
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	}
}

/*
 * @brief Task to initiate Host PC UART RX
 */
void HostPC_RX_Task(void *pvParameters)
{

	request_hostPC_read(); /* Start the first read */
	vTaskDelete(NULL);	   /* Delete this task as it is no longer needed */
	UNUSED(pvParameters);
}
