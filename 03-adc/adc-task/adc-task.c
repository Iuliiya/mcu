#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "hardware/gpio.h"
#include <math.h>

const uint PIN_ADC=26;
const uint channel_of_ADC = 0;

const float razr=12;
void adc_task_init(){
    adc_init();
    adc_gpio_init(PIN_ADC);
}
float adc_measurement(){
    adc_select_input(channel_of_ADC);
    uint16_t voltage_counts = adc_read();
    float voltage_V = voltage_counts / (powf(2, razr)) * 3.3f;
    
    return voltage_V;
}


