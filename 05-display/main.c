#include "stdio.h"
#include "pico/stdlib.h"
#include "stdlib.h"
#include <string.h>
//#include "hardware/gpio.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"
//#include "display/display.h"

#include "hardware/spi.h"
#include "ili9341-driver.h"
#include "ili9341-display.h"
#include "ili9341-font.h"
//#include "display.h"

#define ILI9341_PIN_MISO 4
#define ILI9341_PIN_CS 10
#define ILI9341_PIN_SCK 6
#define ILI9341_PIN_MOSI 7
#define ILI9341_PIN_DC 8
#define ILI9341_PIN_RESET 9
static const uint LED_PIN = 25;
// #define PIN_LED -> 3.3V

ili9341_hal_t ili9341_hal = {0};
static ili9341_display_t ili9341_display = {0};

void rp2040_spi_write(const uint8_t *data, uint32_t size)
{
	spi_write_blocking(spi0, data, size);
}

void rp2040_spi_read(uint8_t *buffer, uint32_t length)
{
	spi_read_blocking(spi0, 0, buffer, length);
}
void rp2040_gpio_cs_write(bool level)
{
	gpio_put(ILI9341_PIN_CS, level);
}
void rp2040_gpio_dc_write(bool level){
    gpio_put(ILI9341_PIN_DC, level);
}
void rp2040_gpio_reset_write(bool level){
    gpio_put(ILI9341_PIN_RESET, level);
}
void rp2040_delay_ms(uint32_t ms){
    sleep_ms(ms);
}

void display_init(){
    gpio_set_function(ILI9341_PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(ILI9341_PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(ILI9341_PIN_SCK, GPIO_FUNC_SPI);

    gpio_init(ILI9341_PIN_CS);
    gpio_init(ILI9341_PIN_DC);
    gpio_init(ILI9341_PIN_RESET);

    gpio_set_dir(ILI9341_PIN_CS, GPIO_OUT);
    gpio_set_dir(ILI9341_PIN_DC, GPIO_OUT);
    gpio_set_dir(ILI9341_PIN_RESET, GPIO_OUT);

    gpio_put(ILI9341_PIN_CS, 1);
    gpio_put(ILI9341_PIN_DC, 0);
    gpio_put(ILI9341_PIN_RESET, 0);
    
    ili9341_hal.spi_write = rp2040_spi_write;
    ili9341_hal.spi_read = rp2040_spi_read;
    ili9341_hal.gpio_cs_write = rp2040_gpio_cs_write;
    ili9341_hal.gpio_dc_write = rp2040_gpio_dc_write;

    ili9341_hal.gpio_reset_write = rp2040_gpio_reset_write;
    ili9341_hal.delay_ms = rp2040_delay_ms;

    ili9341_init(&ili9341_display, &ili9341_hal);
    ili9341_set_rotation(&ili9341_display, ILI9341_ROTATION_90);

    ili9341_fill_screen(&ili9341_display, COLOR_BLACK);
    sleep_ms(300);
    ili9341_draw_filled_rect(&ili9341_display, 10, 10, 100, 60, COLOR_RED);
    ili9341_draw_filled_rect(&ili9341_display, 120, 10, 100, 60, COLOR_GREEN);
    ili9341_draw_filled_rect(&ili9341_display, 230, 10, 80, 60, COLOR_BLUE);

    ili9341_draw_rect(&ili9341_display, 10, 90, 300, 80, COLOR_YELLOW);

    ili9341_draw_line(&ili9341_display, 0, 0, 319, 239, COLOR_YELLOW);
    ili9341_draw_line(&ili9341_display, 319, 0, 0, 239, COLOR_CYAN);

    ili9341_draw_text(&ili9341_display, 20, 100, "Hello, ILI9341!", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);

    ili9341_draw_text(&ili9341_display, 20, 116, "RP2040 / Pico SDK", &jetbrains_font, COLOR_YELLOW, COLOR_BLACK);

}


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
void mem_callback(const char* args) {
    uint32_t addr=0;
    sscanf(args, "%x", &addr);
    uint32_t value = *(uint32_t*)addr;
    printf("0x%08X: 0x%08X\n", addr, value);
}
void disp_screen_callback(const char* args){
	uint32_t c = 0;
	int result = sscanf(args, "%x", &c);
	
	uint16_t color = COLOR_BLACK;
	
	if (result == 1){
		color = RGB888_2_RGB565(c);
	}
	
	ili9341_fill_screen(&ili9341_display, color);
}
void disp_px_callback(const char* args){
    uint32_t c = 0;
    uint16_t x, y;
	int result = sscanf(args, "%hu %hu %x", &x, &y, &c);
	
	uint16_t color = COLOR_BLACK;
	
	if (result == 3){
		color = RGB888_2_RGB565(c);
        ili9341_draw_pixel(&ili9341_display, x , y , color);
	}
    else{
        printf("Что-то пошло не так ¯\\_(ツ)_/¯\n");
    }
}
void disp_line_callback(const char* args){
    uint32_t c = 0;
    uint16_t x0, y0, x1, y1;
	int result = sscanf(args, "%hu %hu %hu %hu %x", &x0, &y0, &x1, &y1, &c);
	
	uint16_t color = COLOR_BLACK;
	
	if (result == 5){
		color = RGB888_2_RGB565(c);
        ili9341_draw_line(&ili9341_display, x0 , y0, x1, y1, color);
        return;
	}
    if (result == 4){
        ili9341_draw_line(&ili9341_display, x0 , y0, x1, y1, color);
        return;
    }
    printf("Что-то пошло не так ¯\\_(ツ)_/¯\n");
}
void disp_rect_callback(const char* args){
   uint32_t c = 0;
    uint16_t x0, y0, width, height;
	int result = sscanf(args, "%hu %hu %hu %hu %x", &x0, &y0, &width, &height, &c);
	
	uint16_t color = COLOR_BLACK;
	
	if (result == 5){
		color = RGB888_2_RGB565(c);
        ili9341_draw_rect(&ili9341_display, x0 , y0, width, height, color);
	}
    if (result == 4){
        ili9341_draw_rect(&ili9341_display, x0 , y0, width, height, color);
    }
    else{
        printf("Что-то пошло не так ¯\\_(ツ)_/¯\n");
    }
}
void disp_frect_callback(const char* args){
    uint32_t c = 0;
    uint16_t x0, y0, width, height;
	int result = sscanf(args, "%hu %hu %hu %hu %x", &x0, &y0, &width, &height, &c);
	
	uint16_t color = COLOR_BLACK;
	
	if (result == 5){
		color = RGB888_2_RGB565(c);
        ili9341_draw_filled_rect(&ili9341_display, x0 , y0, width, height, color);
	}
    if (result == 4){
        ili9341_draw_filled_rect(&ili9341_display, x0 , y0, width, height, color);
    }
    else{
        printf("Что-то пошло не так ¯\\_(ツ)_/¯\n");
    }
}
void disp_text_callback(const char* args){
    uint32_t c = 0;
    uint32_t bc = 0;
    uint16_t x, y;
    char text_[64]={0};
    uint16_t color = COLOR_WHITE;
    uint16_t bcolor = COLOR_BLACK;
    const ili9341_font_t* font = &jetbrains_font;
    int result = sscanf(args, "%hu %hu %63s %x %x", &x, &y, text_, &c, &bc);
    if (c != 0) {
        color=RGB888_2_RGB565(c);
    }
    if (bc != 0) {
        bcolor= RGB888_2_RGB565(bc);
    }
    ili9341_draw_text(&ili9341_display, x, y, text_, font, color, bcolor);
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
    {"disp_screen", disp_screen_callback, "Fill in the color"},
    {"disp_px", disp_px_callback, "Draw in pixels, args: x, y, color"},
    {"disp_line", disp_line_callback, "Draw a line,  args: x0, y0, x1, y1, color"},
    {"disp_rect", disp_rect_callback, "Draw a rect, args: x0, y0, wight, height, color"},
    {"disp_frect", disp_frect_callback, "Draw a filled rect,  args: x0, y0, wight, height, color"},
    {"disp_text", disp_text_callback, "Draw a text, args: x, y, text, text color, background color "},
	{NULL, NULL, NULL},
};

int main(){
    stdio_init_all();
    stdio_task_init();
    protocol_task_init(device_api);
    led_task_init();

    spi_init(spi0, 62500000);
    display_init();

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