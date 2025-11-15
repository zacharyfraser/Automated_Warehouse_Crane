/**
 * @file Comm_Datalink.h
 */

#ifndef COMM_DATALINK_H_
#define COMM_DATALINK_H_

#include <stdint.h>

typedef struct Message
{
    char command[32];
    char arguments[6][16];
    uint8_t arg_count;
} Message_t;

void Tokenize_Task(void *pvParameters);

#endif /* COMM_DATALINK_H_ */