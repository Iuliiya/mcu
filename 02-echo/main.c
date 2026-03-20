#include "stdio.h"
#include "pico/stdlib.h"
#include "stdlib.h"
//#include "hardware/gpio.h"

#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"


int main(){
    stdio_init_all();
    while (1) {
        char symbol = getchar();
        printf("ASCII код: %d\n", symbol);
    }
}