#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "main.h"

void MX_FREERTOS_Init(void);

/**
 * @brief User main function to initialize and start the RTOS kernel.
 */
void user_main(void)
{
    osKernelInitialize(); /* Call init function for freertos objects (in cmsis_os2.c) */
    MX_FREERTOS_Init();   /* CubeMX Generated FreeRTOS objects are initialized here */
    osKernelStart();      /* Start scheduler */
}