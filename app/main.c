/* Setup RCC for the board and the peripherals*/
/* Setup and control the GPIO*/


/* 0x40021000 to 0x400213FF ==> RCC registers*/
/* default setup for clock is none.*/

/* Internal Peripherals of cortex M3 are at 0xE0000000 to 0xE0100000.*/
/* Systick is at 0xE000E010.*/

#include "usart.h"
#include "kernel.h"
#include "gpio.h"
#include "systick.h"

/* SYSTICK */
#define SYSTICK_BASE (0xE000E010)
#define STK_CTRL (*(volatile uint32_t *)(SYSTICK_BASE))
#define STK_LOAD (*(volatile uint32_t *)(SYSTICK_BASE + 0x4))
#define STK_VAL (*(volatile uint32_t *)(SYSTICK_BASE + 0x8))
#define STK_CALIB (*(volatile uint32_t *)(SYSTICK_BASE + 0xC))

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
void uart_task(void){
    uint8_t num = 'A';
    while(1){
        usart_write(&num,1);
        kernel_task_delay(1000);
    }
}


int main(){
    kernel_init();
    usart_enable();
    gpio_enable();
    systick_enable();
    
    gpio_configure(portA, 5, 3, 0); 
    gpio_configure(portA, 2, 3, 2);
    gpio_configure(portA, 3, 0, 1);

    kernel_task_add(uart_task);
    kernel_task_add(led_task);
    kernel_start();

    while(1){
    }

    return 0;
}
