/**
 * @file    user_main.h
 *
 * @brief   Header file for user_main.c
 *
 * Created on: November 11, 2025
 */

#ifndef USER_MAIN_H
#define USER_MAIN_H

#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t Command_Queue;

typedef enum PC_COMMANDS
{
    CMD_CHANGE_MODE = 0,
}PC_Commands_t;

typedef enum MODE_TYPES
{
    MODE_CALI = 0,
    MODE_MANU = 1,
    MODE_AUTO = 2,
}Mode_Types_t;

void user_main(void);

#endif /* USER_MAIN_H */