#include "stdio.h"
#include "pico/stdlib.h"
#include "stdlib.h"
//#include "hardware/gpio.h"
#include "stdio-task/stdio-task.h"

#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"


int main(){
    stdio_init_all();
    stdio_task_init();

    while (1) {
        /*char symbol = getchar();
        printf("ASCII код: %d\n", symbol);*/

        stdio_task_handle();
        
    }
    return 0;
}