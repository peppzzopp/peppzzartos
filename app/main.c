/* Setup RCC for the board and the peripherals*/
/* Setup and control the GPIO*/


/* 0x40021000 to 0x400213FF ==> RCC registers*/
/* default setup for clock is none.*/

/* Internal Peripherals of cortex M3 are at 0xE0000000 to 0xE0100000.*/
/* Systick is at 0xE000E010.*/

#include <stdint.h>
#include "usart.h"
#include "scheduler.h"

#define RCC_BASE (0x40021000)
#define APB2_OFFSET (0x18)
#define RCC_APB2 (*(volatile uint32_t *)(RCC_BASE + APB2_OFFSET))

#define GPIO_BASE (0x40010800)
#define PORT_SIZE (0x400)
#define GPIO_PA_CRL (*(volatile uint32_t *)(GPIO_BASE))
#define GPIO_PA_CRH (*(volatile uint32_t *)(GPIO_BASE + 0x4))
#define GPIO_PA_IDR (*(volatile uint32_t *)(GPIO_BASE + 0x8))
#define GPIO_PA_ODR (*(volatile uint32_t *)(GPIO_BASE + 0xC))

#define SYSTICK_BASE (0xE000E010)
#define STK_CTRL (*(volatile uint32_t *)(SYSTICK_BASE))
#define STK_LOAD (*(volatile uint32_t *)(SYSTICK_BASE + 0x4))
#define STK_VAL (*(volatile uint32_t *)(SYSTICK_BASE + 0x8))
#define STK_CALIB (*(volatile uint32_t *)(SYSTICK_BASE + 0xC))

static volatile uint32_t ticks = 0;

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
        GPIO_PA_ODR ^= (1<<5);
        delay_function(500000);
    }
}
void uart_task(void){
    uint8_t num = 'A';
    while(1){
        usart_write(&num,1);
        delay_function(500000);
    }
}

proc_t led_process = (proc_t){
    .stack_pointer=0,
    .priority=0,
    .index=0,
    .function=led_task
};

proc_t uart_process = (proc_t){
    .stack_pointer=0,
    .priority=0,
    .index=0,
    .function=uart_task
};

int main(){
    scheduler_init();
    usart_enable();
    RCC_APB2 |= ((uint32_t)1 << 2);
    GPIO_PA_CRL &= ~((uint32_t)0xF << 20); 
    GPIO_PA_CRL |= ((uint32_t)0b11 << 20);
    GPIO_PA_CRL |= ((uint32_t)0b00 << 22);
   
    /*USART specific GPIO setup*/
    /*Tx (PA2)--> AF push-pull.*/
    /*Rx (PA3)--> Input float.*/
    GPIO_PA_CRL &= ~((uint32_t)0xFF << 8);
    GPIO_PA_CRL |= ((uint32_t)0b01 << 14);
    GPIO_PA_CRL |= ((uint32_t)0x9 << 8);

    uint32_t ten_ms_HCLK_by_8 = STK_CALIB & ~0xFF000000;
    STK_CTRL = (uint32_t)0b011;
    STK_VAL = 0;
    uint32_t reload_value = ten_ms_HCLK_by_8/10;
    STK_LOAD = reload_value - 1;
    
    uint8_t data[10];
    for(uint32_t i=0; i<10; i++){
        data[i]=i;
    }
    uint8_t led_command = 0;
    
    scheduler_create_process(&uart_process);
    scheduler_create_process(&led_process);
    scheduler_start();

    while(1){
    }

    return 0;
}
