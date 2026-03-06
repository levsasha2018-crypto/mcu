#include <stdio.h>
#include <stdint.h>

#include "pico/stdlib.h"

#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"
#include "adc-task/adc-task.h"

#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN  "v0.0.1"

void help_callback(const char* args);
void version_callback(const char* args);
void led_on_callback(const char* args);
void led_off_callback(const char* args);
void led_blink_callback(const char* args);
void led_blink_set_period_ms_callback(const char* args);
void mem_callback(const char* args);
void wmem_callback(const char* args);
void get_adc_callback(const char* args);
void get_temp_callback(const char* args);
void tm_start_callback(const char* args);
void tm_stop_callback(const char* args);

api_t device_api[] =
{
    {"help",       help_callback, "show available commands"},
    {"version",    version_callback, "get device name and firmware version"},
    {"on",         led_on_callback, "turn led on"},
    {"off",        led_off_callback, "turn led off"},
    {"blink",      led_blink_callback, "blink led"},
    {"set_period", led_blink_set_period_ms_callback, "set blink period in ms"},
    {"mem",        mem_callback, "read 32-bit word from address (hex)"},
    {"wmem",       wmem_callback, "write 32-bit word to address (hex)"},
    {"get_adc",    get_adc_callback, "measure voltage on GPIO26"},
    {"get_temp",   get_temp_callback, "measure RP2040 temperature"},
    {"tm_start",   tm_start_callback, "start adc telemetry"},
    {"tm_stop",    tm_stop_callback, "stop adc telemetry"},
    {NULL, NULL, NULL},
};

void help_callback(const char* args)
{
    (void)args;

    printf("Available commands:\n");

    for (int i = 0; device_api[i].command_name != NULL; i++)
    {
        printf("  %s - %s\n",
               device_api[i].command_name,
               device_api[i].command_help);
    }
}

void version_callback(const char* args)
{
    (void)args;

    printf("device name: '%s', firmware version: %s\n",
           DEVICE_NAME,
           DEVICE_VRSN);
}

void led_on_callback(const char* args)
{
    (void)args;
    led_task_state_set(LED_STATE_ON);
    printf("led turned ON\n");
}

void led_off_callback(const char* args)
{
    (void)args;
    led_task_state_set(LED_STATE_OFF);
    printf("led turned OFF\n");
}

void led_blink_callback(const char* args)
{
    (void)args;
    led_task_state_set(LED_STATE_BLINK);
    printf("led blinking\n");
}

void led_blink_set_period_ms_callback(const char* args)
{
    uint32_t period_ms = 0;

    if (sscanf(args, "%u", &period_ms) != 1 || period_ms == 0)
    {
        printf("error: invalid period\n");
        return;
    }

    led_task_set_blink_period_ms(period_ms);
    printf("blink period set to %u ms\n", period_ms);
}

void mem_callback(const char* args)
{
    uint32_t addr = 0;

    if (sscanf(args, "%x", &addr) != 1)
    {
        printf("error: invalid address\n");
        return;
    }

    uint32_t value = *((volatile uint32_t*)addr);
    printf("mem[0x%08X] = 0x%08X\n", addr, value);
}

void wmem_callback(const char* args)
{
    uint32_t addr = 0;
    uint32_t value = 0;

    if (sscanf(args, "%x %x", &addr, &value) != 2)
    {
        printf("error: invalid arguments\n");
        return;
    }

    *((volatile uint32_t*)addr) = value;
    printf("mem[0x%08X] <= 0x%08X\n", addr, value);
}

void get_adc_callback(const char* args)
{
    (void)args;

    float voltage_V = adc_task_measure_voltage();
    printf("%f\n", voltage_V);
}

void get_temp_callback(const char* args)
{
    (void)args;

    float temp_C = adc_task_measure_temperature();
    printf("%f\n", temp_C);
}

void tm_start_callback(const char* args)
{
    (void)args;

    adc_task_set_state(ADC_TASK_STATE_RUN);
    printf("telemetry started\n");
}

void tm_stop_callback(const char* args)
{
    (void)args;

    adc_task_set_state(ADC_TASK_STATE_IDLE);
    printf("telemetry stopped\n");
}

int main()
{
    stdio_init_all();
    sleep_ms(2000);

    stdio_task_init();
    protocol_task_init(device_api);
    led_task_init();
    adc_task_init();

    while (true)
    {
        char* command = stdio_task_handle();
        protocol_task_handle(command);
        led_task_handle();
        adc_task_handle();
    }

    return 0;
}