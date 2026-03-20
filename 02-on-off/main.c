#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "stdio.h"
#include "stdlib.h"

#include "main.h"
#include "led/led.h"

int main(){
    stdio_init_all();
    led_init();
    while (1) {
        char symbol = getchar();
        switch(symbol){
            case 'e':
                led_enable();
                printf("led enable done\n");
                break;
            case 'd':
                led_disable();
                printf("led disable done\n");
                break;
            case 'v':
                printf("---------------------------------------------------------------\n");
                printf(DEVICE_NAME);
                printf("\n");
                printf(DEVICE_VRSN);
                printf("\n");
                printf("---------------------------------------------------------------\n");
                break;
            default:
                printf("Nakosiachil\n");
                break;
        }
    }
}