/**
 * @file Control_Loop.h
 */

#ifndef CONTROL_LOOP_H
#define CONTROL_LOOP_H

#include <stdint.h>

void Control_Loop_Task(void *pvParameters);
void Update_Motor_Setpoint_Task(void *pvParameters);
void Set_Setpoint(uint32_t setpoint_mm);

#endif /* CONTROL_LOOP_H */