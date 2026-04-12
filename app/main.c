/*
 * This shell is built with demonstrating peppzzartos as its sole purpose.
 * AUTHOR: PEPPZZOPP
 */

/*-------------------------------------------------------------------------------------------------------------------------- includes*/
#include "usart.h"
#include "kernel.h"
#include "systick.h"
#include "gpio.h"
#include "sync/mutex.h"

/*--------------------------------------------------------------------------------------------------------------------------- helpers*/
typedef struct{
    uint8_t data[64];
    uint8_t input_index;
    uint8_t output_index;
    uint32_t mask;
}cbuff_t;

bool write_into(cbuff_t *circular_buffer, uint8_t data_in){
    if(((circular_buffer->input_index + 1)&circular_buffer->mask) == circular_buffer->output_index){
        return false;
    }
    circular_buffer->data[circular_buffer->input_index] = data_in;
    circular_buffer->input_index = (circular_buffer->input_index + 1)&circular_buffer->mask;
    return true;
}

bool read_from(cbuff_t *circular_buffer, uint8_t *data_out){
    if(circular_buffer->input_index == circular_buffer->output_index){
        return false;
    }
    *data_out = circular_buffer->data[circular_buffer->output_index];
    circular_buffer->output_index = (circular_buffer->output_index + 1)&circular_buffer->mask;
    return true;
}

bool str_comp(uint8_t *one, uint8_t *two){
    while(*one && *two){
        if(*one != *two) return false;
        one++; two++;
    }
    return *one == *two;
}

/*------------------------------------------------------------------------------------------------------------------- shell constants*/
static cbuff_t input_buffer = (cbuff_t){.data={0}, .input_index=0, .output_index=0, .mask=(((uint32_t)1 << 6) - 1)};
static cbuff_t command_buffer = (cbuff_t){.data={0}, .input_index=0, .output_index=0, .mask=(((uint32_t)1 << 6) - 1)};
static cbuff_t echo_buffer = (cbuff_t){.data={0}, .input_index=0, .output_index=0, .mask=(((uint32_t)1 << 6) - 1)};
static cbuff_t output_buffer = (cbuff_t){.data={0}, .input_index=0, .output_index=0, .mask=(((uint32_t)1 << 6) - 1)};
static bool command_ready;
static bool output_ready;
static uint8_t instruction[32];
static bool instruction_ready;

/*-------------------------------------------------------------------------------------------------------------------- shell commands*/
typedef struct{
    void (*function)(void);
    uint8_t *command;
}command_t;

void write_string(uint8_t *string, cbuff_t *buffer){
    while(*string){
        if(write_into(buffer, *string)){
            string++;
        } else{
            output_ready = true;
            kernel_task_delay(10);
        }
    }
}

void write_number(uint32_t number, cbuff_t *buffer){
    static uint8_t digits[10];
    uint32_t count = 0;
    if(number == 0){ 
        write_into(buffer, '0'); return; 
    }
    while(number > 0){ 
        digits[count++] = '0' + (number % 10); 
        number /= 10; 
    }
    while(count > 0){
        write_into(buffer, digits[--count]);
    }
}

void not_valid(void){
    static uint8_t *invalid_output = "This is not a valid instruction. Please try again.\n\r";
    uint8_t *command_output;
    command_output = invalid_output;
    write_string(command_output, &output_buffer);
}

void uptime(void){
    /*Output the total time kernel is up and running.*/
    static uint8_t *time_output = "Kernel is up for over ";
    uint8_t *command_output = time_output;
    uint32_t milliseconds = kernel_get_time();
    uint32_t seconds = milliseconds / 1000;
    milliseconds = milliseconds % 1000;
    write_string(command_output, &output_buffer);
    write_number(seconds, &output_buffer);
    write_into(&output_buffer, '.');
    write_number(milliseconds, &output_buffer);
    write_string("s\r\n", &output_buffer);
}

void banner(void){
    /*Output opening banner.*/
    static uint8_t *banner = "Hello!!\n\rWelcome to the demo of peppzzartos.\n\rTake your time watching the screen and when you are done type 'help' to see available commands.\n\r";
    uint8_t *command_output = banner;
    write_string(command_output, &output_buffer);
}

void help(void);

uint32_t led_delay = 1000;
void led_task(void){
    while(1){
        gpio_toggle(portA, 5);
        kernel_task_delay(led_delay);
    }
}

void blink(void){
    led_delay = 0;
    static uint32_t blink_index = -20;
    if(blink_index != -20){
        kernel_task_delete(blink_index);
    }
    uint8_t delay_number[10];
    bool data_available = false;
    uint32_t count = 0;
    do{
        data_available = read_from(&command_buffer, &delay_number[count]);
        count++;
    } while(data_available);
    for(uint32_t i=0; i<count-1; i++){
        led_delay = led_delay*10 + (uint32_t)(delay_number[i] - '0');
    }
    blink_index = kernel_task_add(led_task);
}

static command_t command_table[] = {
    (command_t){.function=blink, .command="blink"},
    (command_t){.function=uptime, .command="uptime"},
    (command_t){.function=help, .command="help"}
};

void help(void){
    static uint8_t *help_output = "The available commands:\n\r";
    uint8_t *command_output = help_output;
    write_string(command_output, &output_buffer);
    for(uint32_t i=0; i<(sizeof(command_table)/sizeof(command_t)); i++){
        write_string(command_table[i].command, &output_buffer);
        write_string("\n\r", &output_buffer);
    }
}

/*----------------------------------------------------------------------------------------------------------------- shell source code*/

void input_task(void){
    while(1){
        static uint8_t buffer[32];
        uint32_t recieved = usart_read(buffer, sizeof(buffer));
        if(recieved > 0){
            for(uint32_t i=0; i<recieved; i++){
                write_into(&input_buffer, buffer[i]);
            }
        }
        kernel_task_delay(10);
    }
}

void format_task(void){
    while(1){
        static bool data_available;
        do{
            static uint8_t data;
            data_available = read_from(&input_buffer, &data);
            if(data_available){
                if(data == '\b'){
                    write_into(&echo_buffer, data);
                } else if((data == '\n') || (data == '\r')){
                    write_into(&echo_buffer, '\n');
                    write_into(&echo_buffer, '\r');
                    command_ready = true;
                } else{
                    write_into(&command_buffer, data);
                    write_into(&echo_buffer, data);
                }
            }
        } while(data_available);
        kernel_task_delay(10);
    }
}

void command_task(void){
    while(1){
        if(command_ready){
            for(uint32_t i=0; i<32; i++){
                instruction[i] = 0;
            }
            uint32_t instruction_index = 0;
            bool data_available = false;
            do{
                uint8_t data;
                data_available = read_from(&command_buffer, &data);
                if(!data_available){
                    if(instruction_index == 0){
                        instruction_ready = false;
                        command_ready = false;
                        break;
                    }
                    instruction[instruction_index] = 0;
                    instruction_ready = true;
                    break;
                }
                if(data == ' '){
                    instruction[instruction_index] = 0;
                    instruction_ready = true;
                    break;
                } else{
                    instruction[instruction_index] = data;
                    instruction_index++;
                }
            } while(data_available);
            command_ready = false;
        }
        kernel_task_delay(10);
    }
}

void dispatch_task(void){
    while(1){
        if(instruction_ready){
            bool instruction_valid = false;
            for(uint32_t i=0; i<(sizeof(command_table)/sizeof(command_t)); i++){
                if(str_comp(command_table[i].command, instruction)){
                    instruction_valid = true;
                    command_table[i].function();
                    break;
                }
            }
            if(!instruction_valid){
                not_valid();
            }
            output_ready = true;
            instruction_ready = false;
        }
        kernel_task_delay(10);
    }
}

void echo_task(void){
    while(1){
        if(!output_ready){
            static bool data_available;
            do{
                static uint8_t data;
                data_available = read_from(&echo_buffer, &data);
                if(data_available) usart_write(&data, 1);
            } while(data_available);
        }
        kernel_task_delay(10);
    }
}

void output_task(void){
    while(1){
        if(output_ready){
            bool data_available;
            do{
                uint8_t data;
                data_available = read_from(&output_buffer, &data);
                if(data_available) usart_write(&data, 1);
            } while(data_available);
            output_ready = false;
        }
        kernel_task_delay(10);
    }
}

int main(void){
    usart_enable();
    kernel_init();
    systick_enable();
    gpio_enable();

    gpio_configure(portA, 2, 1, 2);
    gpio_configure(portA, 3, 0, 1);
    gpio_configure(portA, 5, 1, 0); 
    
    kernel_task_add(input_task);
    kernel_task_add(format_task);
    kernel_task_add(echo_task);
    kernel_task_add(output_task);
    kernel_task_add(command_task);
    kernel_task_add(dispatch_task);


    kernel_start();

    while(1){
    }
    return 0;
}
