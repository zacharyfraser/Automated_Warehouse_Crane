/**
 * @file Ultrasonic_Driver.c
 *
 * @brief Sends periodic trigger pulses to ultrasonic sensors and measures echo pulse durations.
 */

/* Module Header */
#include "L1/Ultrasonic_Driver.h"

/* Standard Libraries */

/* User Libraries */
#include "user_main.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
QueueHandle_t Ultrasonic_Raw_Queue;

/**
 * @brief Task to send trigger pulses and measure echo durations from ultrasonic sensors.
 *
 * Sends a trigger pulse every 100 ms and measures the echo pulse duration using input capture.
 * The measured duration is sent to a queue for further processing.
 */
void Ultrasonic_Read_Task(void *pvParameters)
{
    /* Prepare Echo Timer */
    HAL_TIM_IC_Start(&htim3, TIM_CHANNEL_1);
    __HAL_TIM_ENABLE(&htim3);

    /* Prepare Trigger Timer */
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

    while (true)
    {
        /* Generate trigger pulse */
        __HAL_TIM_ENABLE(&htim2);

        /* Wait for pulse to complete */
        vTaskDelay(pdMS_TO_TICKS(100));

        uint32_t pulse_width_us;
        uint32_t distance_mm;

        /* Read captured pulse width in timer ticks (1 tick = 1 us) */
        pulse_width_us = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1);

        /* Convert pulse width to distance in mm using integer math */
        /* distance_mm = pulse_width_us * speed_of_sound_mm_per_us / 2 */
        /* speed_of_sound ≈ 0.343 mm/us → multiply by 343 and divide by 1000 for mm */
        distance_mm = (pulse_width_us * 343) / 2000; // divide by 2 for round-trip

        /* Send distance to queue */
        xQueueSend(Ultrasonic_Raw_Queue, &distance_mm, portMAX_DELAY);
    }

    UNUSED(pvParameters);
}
