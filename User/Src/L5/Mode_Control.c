/**
 * @file Mode_Control.c
 *
 * @brief Sets the operational mode of the crane.
 *
 * Modes:
 * - Manual: Direct motor control via switches and buttons.
 * - Calibration: Calibrates maximum speed for vertical sensor.
 * - Automatic: Uses closed-loop control to move item from lower to upper shelf.
 */

/* Module Header */
#include "L5/Mode_Control.h"
#include "L4/Manual_Mode.h"
#include "L4/Calibrate_Mode.h"
#include "L4/Auto_Mode.h"

/* Standard Libraries */

/* User Libraries */
#include "user_main.h"
#include <complex.h>
#include "Mode_Control.h"

#define MODE_CONTROL_TASK_DELAY_MS 10

static Control_Mode_t current_mode = MODE_MANUAL;

/**
 * @brief Task to manage control mode of the crane.
 */
void Mode_Control_Task(void *pvParameters)
{
    Initialize_Manual_Mode();
    Initialize_Calibrate_Mode();
    Initialize_Auto_Mode();
    while (true)
    {
        switch (current_mode)
        {
        case MODE_MANUAL:
            Run_Manual_Mode();
            break;
        case MODE_CALIBRATION:
            Run_Calibrate_Mode();
            break;
        case MODE_AUTOMATIC:
            Run_Auto_Mode();
            break;
        default:
            current_mode = MODE_MANUAL;
            break;
        }
    }
    UNUSED(pvParameters);
}

/**
 * @brief Reset and transition mode
 *
 * @param new_mode New control mode to switch to
 */
void Transition_Mode(Control_Mode_t new_mode)
{
    if (new_mode == current_mode)
    {
        return; /* No Change */
    }
    if (new_mode < MODE_MANUAL || new_mode > MODE_AUTOMATIC)
    {
        return; /* Invalid Mode */
    }
    Initialize_Manual_Mode();
    Initialize_Calibrate_Mode();
    Initialize_Auto_Mode();

    current_mode = new_mode;
}