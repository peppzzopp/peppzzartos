#include <stdint.h>
#include "timer.h"

#define ONE_MS (0x6977)

#define TIMER_COMPARE_LW (*(volatile uint32_t *)0x30000100)
#define TIMER_COMPARE_HW (*(volatile uint32_t *)0x30000104)

void timer_init(void){
    uint32_t current_time;
    
    __asm__ volatile("csrr %0, mcycle" : "=r" (current_time));
    
    TIMER_COMPARE_LW = current_time + ONE_MS;
    TIMER_COMPARE_HW = 0x00000000;
}
