#include "sync/semaphore.h"
#include "kernel.h"
#include "kernel_internal.h"

extern uint32_t ready_tasks;
extern uint32_t current_task;
extern uint32_t allocator;

kernel_semaphore_t kernel_semaphore_create(uint8_t count){
    return (kernel_semaphore_t){
        .count=0,
        .max_count=count,
        .last_task=0,
        .in_line=0
    };
}

void kernel_semaphore_wait(kernel_semaphore_t *semaphore){
    kernel_enter_critical();
    while(1) {
        if(semaphore->count < semaphore->max_count){
            semaphore->count++;
            semaphore->last_task = current_task;
            kernel_exit_critical();
            return;
        } else{
            semaphore->in_line |= ((uint32_t)1 << current_task);
            ready_tasks &= ~((uint32_t)1 << current_task);
            kernel_schedule();
            kernel_exit_critical();
            kernel_yield();
        }
    }
}

void kernel_semaphore_signal(kernel_semaphore_t *semaphore){
    kernel_enter_critical();
    semaphore->in_line &= ~allocator;
    uint32_t boolean = semaphore->in_line >> ((semaphore->last_task + 1)&0x1F);
    uint32_t mask = (boolean) ? (boolean) : semaphore->in_line;
    uint32_t i = (boolean) ? (semaphore->last_task+1)&0x1F : 0;
    while(mask){
        if(mask & 0x1){
            semaphore->in_line &= ~((uint32_t)1 << i);
            ready_tasks |= ((uint32_t)1 << i);
            semaphore->count--;
            kernel_exit_critical();
            return;
        }
        mask >>= 1;
        i = (i+1)&0x1F;
    }
    semaphore->count--;
    kernel_exit_critical();
}

