#include "stdio.h"
#include "pico/stdlib.h"
#include "stdlib.h"
#include <string.h>
//#include "hardware/gpio.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"
#include "adc-task/adc-task.h"

#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"

void version_callback(const char* args)
{
	printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}
void led_off_callback(const char* args) {
    led_task_state_set(LED_STATE_OFF);
    printf("Led_OFF\n");
}

void led_on_callback(const char* args) {
    led_task_state_set(LED_STATE_ON);
    printf("Led_ON\n");
}

void led_blink_callback(const char* args) {
    led_task_state_set(LED_STATE_BLINK);
    printf("Led to ready blink\n");
}
void adc_callback(const char* args) {
    float voltage_V = adc_measurement();
    printf("%f\n", voltage_V);
}
api_t device_api[] =
{
	{"version", version_callback, "get device name and firmware version"},
    {"on", led_on_callback, "Turn on the LED"},
    {"off", led_off_callback, "Turn off the LED"},
    {"blink", led_blink_callback, "Make the LED blink"},
    {"get_adc", adc_callback, "To get voltage"},
	{NULL, NULL, NULL},
};

int main(){
    stdio_init_all();
    stdio_task_init();
    protocol_task_init(device_api);
    led_task_init();
    adc_task_init();
    while (1) {
        
        //protocol_task_handle(stdio_task_handle());
        char* input_string = stdio_task_handle();
        if (input_string != NULL)
        {
            char command_copy[128];
            strcpy(command_copy, input_string);
            
            protocol_task_handle(command_copy);
        }
        led_task_handle();
        
    }
    return 0;
}