#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "hardware/gpio.h"
#include <math.h>

#include "adc-task.h"

const uint PIN_ADC=26;
const uint channel_of_ADC = 0;
const uint channel_of_temp_sensor = 4;
// Телеметрия------------------
uint adc_state = 0;
uint64_t tm_time;
uint ADC_TASK_MEAS_PERIOD_US= 100000;
// ----------------------------
const float razr=12;

void adc_task_init(){
    adc_init();
    adc_gpio_init(PIN_ADC);
    adc_set_temp_sensor_enabled(true);
}
float adc_measurement(){
    adc_select_input(channel_of_ADC);
    uint16_t voltage_counts = adc_read();
    float voltage_V = voltage_counts / (powf(2, razr)) * 3.3f;
    
    return voltage_V;
}
float temp_measurement(){
    adc_select_input(channel_of_temp_sensor);
    uint16_t temp_V = adc_read();
    float voltage_V = temp_V / (powf(2, razr)) * 3.3f;
    float temp_C = 27.0f - (voltage_V - 0.706f) / 0.001721f;
    return temp_C;
}
void adc_task_state_set(adc_task_state_t state) {
    adc_state = state;
    if (state == ADC_TASK_STATE_RUN) {   
        tm_time = time_us_64() + (ADC_TASK_MEAS_PERIOD_US);
    } 
}
void adc_task_handle(){
    switch (adc_state)
    {
    case ADC_TASK_STATE_IDLE:
        break;
    case ADC_TASK_STATE_RUN:
        if (time_us_64() > tm_time)
        {
            tm_time = time_us_64() + (ADC_TASK_MEAS_PERIOD_US);
            float Voltage_V= adc_measurement();
            float Temperature_C= temp_measurement();
            printf("%f %f\n", Voltage_V, Temperature_C);
        }
        break;
    default:
        break;
    }
}