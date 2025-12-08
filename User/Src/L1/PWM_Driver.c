/**
 * @file    PWM_Driver.c
 *
 * @brief   Timer initialization and PWM generation
 *
 * Created on: November 13, 2025
 */

/* Module Header */
#include "L1/PWM_Driver.h"

/* Standard Libraries */

/* User Libraries */
#include "user_main.h"
#include "stm32f4xx_hal_tim.h"

#define IDLE_PULSE_WIDTH_US 1500
#define COUNTERCLOCKWISE_PULSE_WIDTH_US 1553
#define CLOCKWISE_PULSE_WIDTH_US 1378
#define KNOCKER_ON_TIME_MS 40
#define KNOCKER_BASE_MS 100 /* 200 ms base period for 5 Hz minimum knocker duty cycle */
#define PWM_FRAME_MS 10     /* 50 Hz PWM frame rate */

extern TIM_HandleTypeDef htim1;
QueueHandle_t PWM_Queue;

typedef struct
{
    uint32_t active_pulse;
    uint32_t frames_on;
    uint32_t frames_off;
    uint32_t frame_counter;
    uint8_t state_on;
    uint32_t timer_channel;
} Servo_t;

static Servo_t servo_horizontal;
static Servo_t servo_vertical;

static void Servo_Update(Servo_t *servo);
static void Servo_Init(Servo_t *servo, uint32_t timer_channel);
void Set_Servo_Drive(Servo_t *servo, PWM_Direction_t direction, uint16_t duty_cycle);

volatile uint32_t next_pulse_h = IDLE_PULSE_WIDTH_US;
volatile uint32_t next_pulse_v = IDLE_PULSE_WIDTH_US;
volatile uint8_t new_frame_ready = 0;

/**
 * @brief Control PWM timers for servo control
 *
 * Configures PWM timers on TIM1 CH1 and CH2 at 100 Hz
 * Servo drive is set by a lower frequency duty cycle wrapping a constant PWM signal.
 * This allows for more precise control of the servo speed, and overcoming sticktion in the vertical mechanism.
 *
 * Note that the servos are overdriven at 100 Hz in order to increase bandwidth for the knocker mechanism.
 */
void PWM_Timer_Task(void *pvParameters)
{
    /* Timer1 is clocked from APB2 @ 84 MHz */
    /* Make sure external clock from STLink is timer source - HSI has a lot of drift */

    /* Maximum Counter period is 65535
     * Prescaler = (Timer clock / Desired counter clock) - 1
     * Prescaler = (84,000,000 / 1,000,000) - 1 = 83
     * Counter clock = 1 MHz
     * PWM Frequency = Counter clock / (Period + 1) = 1,000,000 / 10000 = 100 Hz
     *
     * Configuration set in CubeMX
     */
    PWM_Duty_Cycle_t cmd;

    Servo_Init(&servo_horizontal, TIM_CHANNEL_2);
    Servo_Init(&servo_vertical, TIM_CHANNEL_1);

    /* Start timer with update interrupt every rollover (10 ms) */
    HAL_TIM_Base_Start_IT(&htim1);

    while (1)
    /* Update Servo Drive Duty Cycles*/
    {
        if (xQueueReceive(PWM_Queue, &cmd, portMAX_DELAY) == pdTRUE)
        {
            if (cmd.channel == HORIZONTAL_SERVO_PWM)
            {
                Set_Servo_Drive(&servo_horizontal, cmd.direction, cmd.duty_cycle);
            }
            else if (cmd.channel == VERTICAL_SERVO_PWM)
            {
                Set_Servo_Drive(&servo_vertical, cmd.direction, cmd.duty_cycle);
            }
        }
    }

    UNUSED(pvParameters);
}

/**
 * @brief Update duty cycle state machine for one servo
 *
 * @param Servo_t *servo Pointer to servo structure
 */
static void Servo_Update(Servo_t *servo)
{
    if (servo->state_on)
    {
        servo->frame_counter++;
        if (servo->frame_counter >= servo->frames_on)
        {
            servo->frame_counter = 0;
            servo->state_on = 0;
        }
    }
    else
    {
        servo->frame_counter++;
        if (servo->frame_counter >= servo->frames_off)
        {
            servo->frame_counter = 0;
            servo->state_on = 1;
        }
    }
}

/**
 * @brief Initialize Servo PWM Driver
 *
 * @param Servo_t *servo Pointer to servo structure
 */
static void Servo_Init(Servo_t *servo, uint32_t timer_channel)
{
    servo->active_pulse = IDLE_PULSE_WIDTH_US;
    servo->frames_on = KNOCKER_ON_TIME_MS / PWM_FRAME_MS;
    if (servo->frames_on == 0)
    {
        servo->frames_on = 1;
    }
    servo->frames_off = 0;
    servo->frame_counter = 0;
    servo->state_on = 1;
    servo->timer_channel = timer_channel;

    /* Start PWM channel */
    __HAL_TIM_SET_COMPARE(&htim1, servo->timer_channel, IDLE_PULSE_WIDTH_US);
    HAL_TIM_PWM_Start(&htim1, servo->timer_channel);
}

/**
 * @brief Set servo drive
 *
 * @param servo Servo to update
 * @param direction Direction enum for servo movement
 * @param duty_cycle Duty cycle percentage (0-100)
 */
void Set_Servo_Drive(Servo_t *servo, PWM_Direction_t direction, uint16_t duty_cycle)
{
    if (duty_cycle > 100)
    {
        duty_cycle = 100;
    }

    uint32_t off_time = (100 - duty_cycle);

    servo->frames_off = off_time / PWM_FRAME_MS;
    servo->frames_on = KNOCKER_ON_TIME_MS / PWM_FRAME_MS;
    if (servo->frames_on == 0)
    {
        servo->frames_on = 1;
    }

    servo->active_pulse = IDLE_PULSE_WIDTH_US;
    if (direction == DIRECTION_CLOCKWISE)
    {
        servo->active_pulse = CLOCKWISE_PULSE_WIDTH_US;
    }
    else if (direction == DIRECTION_COUNTERCLOCKWISE)
    {
        servo->active_pulse = COUNTERCLOCKWISE_PULSE_WIDTH_US;
    }
}

/**
 * @brief Timer period elapsed callback
 *
 * This function is called by the HAL library on every timer rollover (10ms).
 * It updates the servo states and sets the next pulse widths.
 *
 * @param htim Pointer to the TIM handle.
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
        /* Update servo state every pulse (10 ms) */
        Servo_Update(&servo_horizontal);
        Servo_Update(&servo_vertical);

        /* Set next pulse widths */
        next_pulse_h = servo_horizontal.state_on ? servo_horizontal.active_pulse : IDLE_PULSE_WIDTH_US;
        next_pulse_v = servo_vertical.state_on ? servo_vertical.active_pulse : IDLE_PULSE_WIDTH_US;

        /* Update PWM compare registers */
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, next_pulse_v);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, next_pulse_h);
    }
}

/**
 * @brief Disable all PWM outputs and set to idle
 * Used when limit switches are triggered
 */
void PWM_Disable_All(void)
{
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
}