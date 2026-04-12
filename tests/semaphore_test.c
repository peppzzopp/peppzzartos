#include "usart.h"
#include "kernel.h"
#include "gpio.h"
#include "systick.h"
#include "sync/semaphore.h"

kernel_semaphore_t flag;

static void delay_function(uint32_t n){
    volatile uint32_t x = 0;
    for(volatile uint32_t i=0; i<n; i++){
        if(x > 0){
            return;
        }
    }
}

void led_task(void){
    while(1){
        gpio_toggle(portA, 5);
        kernel_task_delay(1000);
    }
}
void uart_task_1(void){
    while(1){
        kernel_semaphore_wait(&flag);
        char str[] = "HELLO WORLD\n";
        for(uint32_t i=0; i<sizeof(str); i++){
            usart_write((uint8_t *)str+i, 1);
            kernel_task_delay(10);
        }
        kernel_semaphore_signal(&flag);
        kernel_task_delay(10);
    }
}

void uart_task_2(void){
    while(1){
        kernel_semaphore_wait(&flag);
        char str[] = "MARK MY WORDS\n";
        for(uint32_t i=0; i<sizeof(str); i++){
            usart_write((uint8_t *)str+i, 1);
            kernel_task_delay(10);
        }
        kernel_semaphore_signal(&flag);
        kernel_task_delay(10);
    }
}
int main(){
    kernel_init();
    usart_enable();
    gpio_enable();
    systick_enable();
    
    flag = kernel_semaphore_create(1);

    gpio_configure(portA, 5, 1, 0); 
    gpio_configure(portA, 2, 1, 2);
    gpio_configure(portA, 3, 0, 1);

    kernel_task_add(uart_task_1);
    kernel_task_add(uart_task_2);
    kernel_task_add(led_task);

    kernel_start();

    while(1){
    }

    return 0;
}
