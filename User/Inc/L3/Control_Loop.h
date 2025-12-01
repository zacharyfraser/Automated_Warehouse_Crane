/**
 * @file Control_Loop.h
 */

#ifndef CONTROL_LOOP_H
#define CONTROL_LOOP_H

void Control_Loop_Task(void *pvParameters);
void Update_Motor_Setpoint_Task(void *pvParameters);

#endif /* CONTROL_LOOP_H */