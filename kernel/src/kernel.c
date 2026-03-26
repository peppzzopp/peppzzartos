#include "kernel.h"
#include "kernel_internal.h"

#define TASK_MEMORY (40)

extern uint32_t heap_start;
static uint32_t allocator;
task_t *task_table[32] = {0};

uint32_t current_task;
uint32_t next_task;

static uint32_t count_1s(uint32_t x){
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    x = x + (x >> 8);
    x = x + (x >> 16);
    
    return x & 0x3F;
}

static void kernel_schedule(void){
    next_task = current_task;
    for(uint32_t i=1; i<32; i++){
        uint32_t index = (current_task + i) & 0x1F;
        if((task_table[index] != 0) && (task_table[index]->state_of_task == 1)){
            next_task = index;
            return;
        }
    }
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
}

void kernel_start(void){
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
        task_table[memory_index - 1]=task;

        task->state_of_task = 1;
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
    for(uint32_t i=0; i<32; i++){
        if(task_table[i]->state_of_task == 0){
            task_table[i]->delay_timer--;
            if(task_table[i]->delay_timer == 0){
                task_table[i]->state_of_task = 1;
            }
        }
    }    
    kernel_schedule();
    if(current_task != next_task){
        kernel_yield();
    }
}

void kernel_task_delay(uint32_t delay){
    task_table[current_task]->delay_timer = delay;
    task_table[current_task]->state_of_task = 0;
    kernel_yield();
}
