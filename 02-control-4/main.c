#include "stdio.h"
#include "pico/stdlib.h"
#include "stdlib.h"
#include <string.h>
//#include "hardware/gpio.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"

#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"
extern api_t device_api[];

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
void led_blink_set_period_ms_callback(const char* args){
    uint period_ms = 0;
    sscanf(args, "%u", &period_ms);
    if (period_ms==0){
        printf("PERIOD 0 AAAAAAAAA\n");
        return;
    }
    led_task_set_blink_period_ms(period_ms);
}
void command_help_callback(){
    api_t* indicator = device_api;
    int i=0;
    while ((indicator+i)->command_name != NULL){
        printf("Команда %s: %s\n", 
               (indicator + i)->command_name,
               (indicator + i)->command_help);
        i++;
    }
}
api_t device_api[] =
{
    {"command_help", command_help_callback, "Get list of function"},
	{"version", version_callback, "get device name and firmware version"},
    {"on", led_on_callback, "Turn on the LED"},
    {"off", led_off_callback, "Turn off the LED"},
    {"blink", led_blink_callback, "Make the LED blink"},
    {"set_period", led_blink_set_period_ms_callback, "Set the value period"},
	{NULL, NULL, NULL},
};

int main(){
    stdio_init_all();
    stdio_task_init();
    protocol_task_init(device_api);
    led_task_init();
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