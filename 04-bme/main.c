#include "stdio.h"
#include "pico/stdlib.h"
#include "stdlib.h"
#include <string.h>
#include "hardware/i2c.h"
//#include "hardware/gpio.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task.h"
#include "led-task/led-task.h"
#include "bme280-driver.h"

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
/*void mem_callback(const char* args){
    uint32_t addr;
    sscanf(arg, "%x", &addr);
    printf(&add);
}*/
void mem_callback(const char* args) {
    uint32_t addr=0;
    sscanf(args, "%x", &addr);
    uint32_t value = *(uint32_t*)addr;
    printf("0x%08X: 0x%08X\n", addr, value);
}
//______________________________________________________________

void rp2040_i2c_read(uint8_t* buffer, uint16_t length)
{
	i2c_read_timeout_us(i2c1, 0x76, buffer, length, false, 100000);
}
void rp2040_i2c_write(uint8_t* data, uint16_t size)
{
	i2c_write_timeout_us(i2c1, 0x76, data, size, false, 100000);
}
void bme280_driver_init(){
    i2c_init(i2c1, 100000); 
	gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    bme280_init(rp2040_i2c_read, rp2040_i2c_write);
}

void read_regs_callback(const char* args){
    uint8_t buffer[256] = {0};
    uint8_t addr = 0;
    uint8_t N = 0;
    
    int result = sscanf(args, "%hhx %hhu", &addr, &N);
    if ((result != 2) || (addr > 0xFF) || (N > 0xFF) || (addr + N > 0x100)) {
        printf("Что-то пошло не так ¯\\_(ツ)_/¯\n");
        return;
    }
    bme280_read_regs(addr, buffer, N);

    for (int i = 0; i < N; i++){
        printf("bme280 register [0x%X] = 0x%X\n", addr + i, buffer[i]);
    }
}

void write_reg_callback(const char* args){
    //uint8_t buffer[256] = {0};
    uint8_t addr = 0;
    uint8_t value = 0;

    int result = sscanf(args, "%hhx %hhx", &addr, &value);

     if ((result != 2) || (addr > 0xFF)){
        printf("Что-то пошло не так ¯\\_(ツ)_/¯\n");
        return;
    }

    bme280_write_reg(addr, value);
    sleep_ms(5);
    uint8_t verify = 0;
    bme280_read_regs(addr, &verify, 1);
    
    if (verify == value) {
        printf("OK: Register 0x%02X = 0x%02X\n", addr, verify);
    } else {
        printf("FAIL: Expected 0x%02X, got 0x%02X\n", value, verify);
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
    {"mem", mem_callback, ""},
    {"read_regs", read_regs_callback, ""},
    {"write_reg", write_reg_callback, "write_reg addr value"},
	{NULL, NULL, NULL},
};

int main(){
    stdio_init_all();
    stdio_task_init();
    protocol_task_init(device_api);
    led_task_init();

    bme280_driver_init();

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