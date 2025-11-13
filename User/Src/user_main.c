#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "main.h"
#include "util.h"
#include <stdio.h>

void MX_FREERTOS_Init(void);

/**
 * @brief User main function to initialize and start the RTOS kernel.
 */


char main_string[50];
uint32_t main_counter = 0;


static void main_task(void *param){
 while(1){
 print_str("Main task loop executing\r\n");
 sprintf(main_string,"Main task iteration: 0x%08lx\r\n", main_counter++);
 print_str(main_string);
 vTaskDelay(1000/portTICK_RATE_MS);
 }
}


void user_main(void)
{
    util_init();
    osKernelInitialize(); /* Call init function for freertos objects (in cmsis_os2.c) */
    MX_FREERTOS_Init();   /* CubeMX Generated FreeRTOS objects are initialized here */
    xTaskCreate(main_task,"Main Task", configMINIMAL_STACK_SIZE + 100, NULL,
tskIDLE_PRIORITY + 2, NULL);
    osKernelStart();      /* Start scheduler */
}