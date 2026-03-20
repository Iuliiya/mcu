#include "stdio.h"
#include "pico/stdlib.h"
#include "stdlib.h"
#include <string.h>
//#include "hardware/gpio.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"

#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"

void version_callback(const char* args)
{
	printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}
api_t device_api[] =
{
	{"version", version_callback, "get device name and firmware version"},
	{NULL, NULL, NULL},
};

int main(){
    stdio_init_all();
    stdio_task_init();
    protocol_task_init(device_api);
    while (1) {
        
        //protocol_task_handle(stdio_task_handle());
        char* input_string = stdio_task_handle();
        if (input_string != NULL)
        {
            char command_copy[128];
            strcpy(command_copy, input_string);
            
            protocol_task_handle(command_copy);
        }
        
    }
    return 0;
}