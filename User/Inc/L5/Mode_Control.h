/**
 * @file Mode_Control.h
 */

#ifndef MODE_CONTROL_H
#define MODE_CONTROL_H
typedef enum
{
    MODE_MANUAL = 0,
    MODE_CALIBRATION,
    MODE_AUTOMATIC
} Control_Mode_t;

void Mode_Control_Task(void *pvParameters);
void Transition_Mode(Control_Mode_t new_mode);

#endif /* MODE_CONTROL_H */