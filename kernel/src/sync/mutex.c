#include "sync/mutex.h"
#include "kernel.h"
#include "kernel_internal.h"

extern uint32_t current_task;
extern uint32_t ready_tasks;
extern uint32_t allocator;

kernel_mutex_t kernel_mutex_create(void){
    return (kernel_mutex_t){
        .owner=0,
        .status=0,
        .in_line=0
    };
}

void kernel_mutex_own(kernel_mutex_t *mutex){
    kernel_enter_critical();
    if(mutex->status == 0){
        mutex->owner = current_task;
        mutex->status = 1;
        kernel_exit_critical();
    } else if(mutex->owner != current_task){
        mutex->in_line |= ((uint32_t)1 << current_task);
        ready_tasks &= ~((uint32_t)1 << current_task);
        kernel_schedule();
        kernel_exit_critical();
        kernel_yield();
    } else{
        kernel_exit_critical();
    }
}

void kernel_mutex_free(kernel_mutex_t *mutex){
    kernel_enter_critical();
    mutex->in_line &= ~allocator;
    uint32_t boolean = mutex->in_line >> ((mutex->owner + 1)&0x1F);
    uint32_t mask = (boolean) ? (boolean) : mutex->in_line;
    uint32_t i = (boolean) ? (mutex->owner + 1)&0x1F : 0;
    while(mask){
        if(mask & 0x1){
            mutex->owner = i;
            mutex->in_line &= ~((uint32_t)1 << i);
            ready_tasks |= ((uint32_t)1 << i);
            kernel_exit_critical();
            return;
        }
        mask >>= 1;
        i = (i+1)&0x1F;
    }
    mutex->owner = 0;
    mutex->status = 0;
    mutex->in_line = 0;
    kernel_exit_critical();
}
