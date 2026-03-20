#include "stdio.h"
#include "pico/stdlib.h"
#include "protocol-task.h"
#include "stdlib.h"
#include "string.h"

//static api_t* api = {0};
static api_t* api = NULL;
static int commands_count = 0;

void protocol_task_init(api_t* device_api){
    api=device_api;
    commands_count=0;
    while ((device_api+commands_count)->command_name!=NULL) {
        commands_count+=1;
    }
}
void protocol_task_handle(char* command_string){
    if (command_string==NULL){
        return;
    } else{
    const char* command_name = command_string;
    const char* command_args = NULL;
    char* space_symbol = strchr(command_string, ' ');
    if (space_symbol)
    {
        *space_symbol = '\0';
        command_args = space_symbol + 1;
    }
    else
    {
        command_args = "";
    }
    
    printf( command_name);
    printf("\n");
    printf(command_args);  
    printf("\n");

    bool found = false;
    for (int i=0; i<commands_count; i++){
        if (strcmp(command_name, api[i].command_name)==0){
            found=true;
            api[i].command_callback(command_args);
            break;
        }
    }
    if (found==false){
        printf("Ошибка! Не найдена программа с именем ");
        printf(command_name);
        printf("\n");
    }
    
    //printf(command_name);
    //printf(command_args);
    printf("______________________________________________________\n");
    }
}