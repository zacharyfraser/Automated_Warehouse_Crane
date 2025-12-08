/**
 * @file    user_main.h
 *
 * @brief   Header file for user_main.c
 *
 * Includes declarations for global FreeRTOS objects and default includes for all user files.
 *
 * Created on: November 11, 2025
 */

#ifndef USER_MAIN_H
#define USER_MAIN_H

#include <stdbool.h>
#include <stdint.h>

#include "FreeRTOS.h"     /* FreeRTOS kernal */
#include "event_groups.h" /* FreeRTOS event group functions */
#include "task.h"         /* FreeRTOS task functions */
#include "queue.h"        /* FreeRTOS queue functions */
#include "semphr.h"       /* FreeRTOS semaphore functions */
#include "main.h"         /* Hal and Object Handles */
#include "util.h"         /* Print Functions */

void user_main(void);

#endif /* USER_MAIN_H */