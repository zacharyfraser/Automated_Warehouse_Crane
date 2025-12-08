/**
 * @file Control_Loop.h
 */

#ifndef CONTROL_LOOP_H
#define CONTROL_LOOP_H

#include <stdint.h>
#include <stdbool.h>

void Control_Loop_Task(void *pvParameters);
void Update_Motor_Setpoint_Task(void *pvParameters);
void Set_Setpoint(uint32_t setpoint_mm);
void Toggle_PID_Control(bool enable);

void Set_Proportional_Gain(float Kp);
void Set_Integral_Gain(float Ki);
void Set_Derivative_Gain(float Kd);
void Print_PID_Gains(void);

#endif /* CONTROL_LOOP_H */