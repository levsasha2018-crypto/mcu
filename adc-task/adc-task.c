#include <stdio.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "adc-task.h"

static const uint ADC_GPIO = 26;
static const uint ADC_INPUT = 0;
static const uint TEMP_SENSOR_ADC_INPUT = 4;

static adc_task_state_t adc_state = ADC_TASK_STATE_IDLE;
static uint64_t adc_meas_ts = 0;
static uint32_t ADC_TASK_MEAS_PERIOD_US = 100000;

void adc_task_init(void)
{
    adc_init();
    adc_gpio_init(ADC_GPIO);
    adc_set_temp_sensor_enabled(true);

    adc_state = ADC_TASK_STATE_IDLE;
    adc_meas_ts = 0;
}

void adc_task_set_state(adc_task_state_t state)
{
    adc_state = state;
}

float adc_task_measure_voltage(void)
{
    adc_select_input(ADC_INPUT);

    uint16_t voltage_counts = adc_read();
    float voltage_V = voltage_counts * 3.3f / 4095.0f;

    return voltage_V;
}

float adc_task_measure_temperature(void)
{
    adc_select_input(TEMP_SENSOR_ADC_INPUT);

    uint16_t temp_counts = adc_read();
    float temp_V = temp_counts * 3.3f / 4095.0f;
    float temp_C = 27.0f - (temp_V - 0.706f) / 0.001721f;

    return temp_C;
}

void adc_task_handle(void)
{
    if (adc_state != ADC_TASK_STATE_RUN)
    {
        return;
    }

    uint64_t now_us = time_us_64();

    if (now_us < adc_meas_ts)
    {
        return;
    }

    adc_meas_ts = now_us + ADC_TASK_MEAS_PERIOD_US;

    uint32_t uptime_ms = to_ms_since_boot(get_absolute_time());
    float voltage_V = adc_task_measure_voltage();
    float temp_C = adc_task_measure_temperature();

    printf("%lu %f %f\n", uptime_ms, voltage_V, temp_C);
}