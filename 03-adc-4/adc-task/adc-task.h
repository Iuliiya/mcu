#pragma once
typedef enum
{
	ADC_TASK_STATE_IDLE = 0,
	ADC_TASK_STATE_RUN = 1,
} adc_task_state_t;

void adc_task_init();
float adc_measurement();
float temp_measurement();
void adc_task_state_set(adc_task_state_t state);
void adc_task_handle();
