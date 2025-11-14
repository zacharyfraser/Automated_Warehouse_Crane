/*
 * util.h
 *
 *  Created on: Aug 8, 2022
 *      Author: Andre Hendricks
 */

#ifndef INC_USER_UTIL_H_
#define INC_USER_UTIL_H_

// ---------- Initialization ----------
void util_init();

// ---------- UART Transmit ----------
void print_str(char *str);
void print_str_ISR(char *str);
void print_str_unsafe(char *str);

// // ---------- UART Receive ----------
// uint8_t uart2_get_char(void);
// int uart2_get_char_nonblocking(uint8_t *ch);
// void uart2_get_line(char *buffer, size_t max_len);

// // ---------- HAL Callback when Interrupt fires----------
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void configure_usart_hostPC(void);                       // Configure Host PC UART and its queue
void request_hostPC_read(void);                          // Start UART reception in interrupt mode
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart); // Callback for UART RX Complete Interrupt

#endif /* INC_USER_UTIL_H_ */
