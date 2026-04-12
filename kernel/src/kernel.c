#include "kernel.h"
#include "kernel_internal.h"

#define TASK_MEMORY (40)

extern uint32_t heap_start;
static uint32_t allocator;
volatile uint32_t ready_tasks;
volatile static uint32_t delayed_tasks;
task_t *task_table[32] = {0};
uint32_t number_of_tasks = 0;

volatile uint32_t current_task;
volatile uint32_t next_task;
volatile uint32_t last_task;

volatile static uint32_t tick_time;

static inline uint32_t count_1s(uint32_t x){
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    x = x + (x >> 8);
    x = x + (x >> 16);
    
    return x & 0x3F;
}

inline void kernel_schedule(void){
    last_task = (current_task == 0) ? last_task : current_task;
    uint32_t mask_pre = ready_tasks & ~(0x1);
    uint32_t mask = (mask_pre >> ((last_task + 1)&0x1F)) ? (mask_pre >> ((last_task + 1)&0x1F)) : mask_pre;
    uint32_t i = (mask_pre >> ((last_task + 1)&0x1F)) ? last_task+1 : 0;
    while(mask){
        if(mask & 1){
            next_task = i;
            return;
        }
        mask >>= 1;
        i++;
    }
    next_task = 0;
    return;
}

static void sleep_function(void){
    while(1){
        ;;
    }
}

void kernel_init(void){
    allocator = 0xFFFFFFFF;
    kernel_task_add(&sleep_function);
    current_task = 0;
    next_task = 0;

    #ifdef PEPPZZARTOS_TIMING_ENABLE
    kernel_timing_init();
    #endif
}

void kernel_start(void){
    tick_time = 0;
    kernel_schedule();
    current_task = 0xFFFFFFFF;
    kernel_yield();
}

int32_t kernel_task_add(void (*function)(void)){
    /*Create a task with the given function*/
    /*Construct a proper stack frame for it*/
    if(allocator != 0x00000000){
        uint32_t memory_index = count_1s(allocator ^ (allocator - 1));
        task_t *task;
        task = (task_t *)((uint32_t *)&heap_start + (memory_index)*TASK_MEMORY);
        task->stack_pointer = (uint32_t *)((uint8_t *)task - sizeof(task_t));
        task->task_function = function;
        allocator &= ~((uint32_t)1 << (memory_index - 1));
        ready_tasks |= ((uint32_t)1 << (memory_index - 1));
        delayed_tasks &= ~((uint32_t)1 << (memory_index - 1));
        task_table[memory_index - 1]=task;

        task->delay_timer = 0;

        task->stack_pointer--;
        *task->stack_pointer = ((uint32_t)1 << 24); /*xPSR*/
        task->stack_pointer--;
        *task->stack_pointer = (uint32_t)task->task_function; /*PC*/
        task->stack_pointer--;
        *task->stack_pointer = 0xFFFFFFFD; /*LR*/
        for(uint32_t i=0; i<13; i++){ /*R0-R12*/
            task->stack_pointer--;
            *task->stack_pointer = 0x00000000;
        }
        return (memory_index-1);
    }
    return -1;
}

bool kernel_task_delete(int32_t index){
    if(index == 0){
        return false;
    }
    if(task_table[index] == 0){
        return false;
    }
    task_table[index] = 0;
    allocator |= ((uint32_t)1 << index);
    return true;
}


void kernel_ticks(void){
    tick_time++;
    uint32_t mask = delayed_tasks;
    uint32_t i = 0;
    while(mask){
        if(mask & 1){
            task_table[i]->delay_timer--;
            if(task_table[i]->delay_timer == 0){
                delayed_tasks &= ~((uint32_t)1 << i);
                ready_tasks |= ((uint32_t)1 << i);
            }
        }
        mask >>= 1;
        i = (i + 1)&0x1F;
    }
    kernel_schedule();
    if(current_task != next_task){
        kernel_yield();
    }
}

void kernel_task_delay(uint32_t delay){
    if(current_task != 0){
        kernel_enter_critical();
        task_table[current_task]->delay_timer = delay;
        delayed_tasks |= ((uint32_t)1 << current_task);
        ready_tasks &= ~((uint32_t)1 << current_task);
        kernel_schedule();
        kernel_exit_critical();
        kernel_yield();
    }
}

uint32_t kernel_get_time(void){
    return tick_time;
}
