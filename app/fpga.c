/*
 * This LED demo is intended to run on peppzzemcu(https://github.com/peppzzopp/peppzzemcu).
 * Main goal here is to test the port of peppzzartos onto a risc-v softcore designed by myself.
 * AUTHOR: PEPPZZOPP
 */

#include "kernel.h"
#include "sync/mutex.h"
#include "timer.h"

#define LED_GPIO (*(volatile uint32_t*)0x30000000)

kernel_mutex_t led_mutex;

void led0_task(void){
    while(1){
        kernel_mutex_own(&led_mutex);
        LED_GPIO ^= (uint32_t)(1);
        kernel_mutex_free(&led_mutex);
        kernel_task_delay(600);
    }
}
void led1_task(void){
    kernel_task_delay(100);
    while(1){
        kernel_mutex_own(&led_mutex);
        LED_GPIO ^= (uint32_t)(1<<1);
        kernel_mutex_free(&led_mutex);
        kernel_task_delay(600);
    }
}
void led2_task(void){
    kernel_task_delay(200);
    while(1){
        kernel_mutex_own(&led_mutex);
        LED_GPIO ^= (uint32_t)(1<<2);
        kernel_mutex_free(&led_mutex);
        kernel_task_delay(600);
    }
}
void led3_task(void){
    kernel_task_delay(300);
    while(1){
        kernel_mutex_own(&led_mutex);
        LED_GPIO ^= (uint32_t)(1<<3);
        kernel_mutex_free(&led_mutex);
        kernel_task_delay(600);
    }
}
void led4_task(void){
    kernel_task_delay(400);
    while(1){
        kernel_mutex_own(&led_mutex);
        LED_GPIO ^= (uint32_t)(1<<4);
        kernel_mutex_free(&led_mutex);
        kernel_task_delay(600);
    }
}
void led5_task(void){
    kernel_task_delay(500);
    while(1){
        kernel_mutex_own(&led_mutex);
        LED_GPIO ^= (uint32_t)(1<<5);
        kernel_mutex_free(&led_mutex);
        kernel_task_delay(600);
    }
}


int main(void){
    kernel_init();
    kernel_task_add(led0_task);
    kernel_task_add(led1_task);
    kernel_task_add(led2_task);
    kernel_task_add(led3_task);
    kernel_task_add(led4_task);
    kernel_task_add(led5_task);
    timer_init();
    kernel_start();
    while(1){
        ;;
    }
}
