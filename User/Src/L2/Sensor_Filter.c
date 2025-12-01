/**
 * @file Sensor_Filter.c
 *
 * @brief Implements filtering for raw sensor data.
 */

/* Module Header */
#include "L2/Sensor_Filter.h"

/* Standard Libraries */

/* User Libraries */
#include "user_main.h"

#define MEDIAN_WINDOW_SIZE 3
#define ALPHA 160 /* Alpha value in Q8 form */
#define Q8_SCALE_FACTOR 256

/* Median filter buffer */
static uint32_t median_buffer[MEDIAN_WINDOW_SIZE];
static uint8_t median_index = 0;

/* Low-pass filtered value */
static uint32_t lowpass_filtered = 0;

QueueHandle_t Filtered_Ultrasonic_Queue;
extern QueueHandle_t Raw_Ultrasonic_Queue;

/**
 * @brief Compute the median of three values.
 *
 * Used to reduce spikes in ultrasonic sensor readings.
 *
 * @param a First value
 * @param b Second value
 * @param c Third value
 * @return Median value
 */
static uint32_t median_of_3(uint16_t a, uint16_t b, uint16_t c)
{
    if ((a > b) != (a > c))
        return a;
    else if ((b > a) != (b > c))
        return b;
    else
        return c;
}

/**
 * @brief Apply a low-pass filter to the input sample.
 *
 * First order IIR filter: y[n] = (alpha * x[n] + (1 - alpha) * y[n-1])
 * fc ~ alpha / (2 * pi * Ts * 256)
 * Uses Q8 fixed-point arithmetic.
 *
 * @param sample New input sample
 * @return Filtered output sample
 */
static uint32_t lowpass(uint16_t sample)
{
    lowpass_filtered = (ALPHA * sample + (Q8_SCALE_FACTOR - ALPHA) * lowpass_filtered) >> 8;
    return lowpass_filtered;
}

/**
 * @brief Task to filter raw ultrasonic sensor readings.
 *
 * Applies median filtering followed by low-pass filtering.
 * Sends filtered readings to Filtered_Ultrasonic_Queue.
 */
void Sensor_Filter_Task(void *pvParameters)
{
    uint32_t raw_sample;
    uint32_t median_value;
    uint32_t filtered_value;

    /* Initialize filters with first sample */
    if (xQueueReceive(Raw_Ultrasonic_Queue, &raw_sample, portMAX_DELAY) == pdPASS)
    {
        lowpass_filtered = raw_sample;
        for (uint8_t i = 0; i < MEDIAN_WINDOW_SIZE; i++)
        {
            median_buffer[i] = raw_sample;
        }
        /* Send initial filtered value */
        xQueueSend(Filtered_Ultrasonic_Queue, &raw_sample, 0);
    }

    while (1)
    {
        if (xQueueReceive(Raw_Ultrasonic_Queue, &raw_sample, portMAX_DELAY) == pdPASS)
        {
            /* Update median buffer */
            median_buffer[median_index++] = raw_sample;
            if (median_index >= MEDIAN_WINDOW_SIZE)
                median_index = 0;

            /* Compute median */
            median_value = median_of_3(median_buffer[0], median_buffer[1], median_buffer[2]);

            /* Low-pass filter */
            filtered_value = lowpass(median_value);

            /* Send filtered value to queue */
            xQueueSend(Filtered_Ultrasonic_Queue, &filtered_value, 0);
        }
    }
}