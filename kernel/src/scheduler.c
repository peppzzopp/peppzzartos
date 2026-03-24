#include "scheduler.h"

#define STACK_FRAME (32)

static proc_t *process_table[32];

static inline uint32_t count_1s(uint32_t x){
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    x = x + (x >> 8);
    x = x + (x >> 16);
    
    return x & 0x3F;
}

extern uint32_t heap_start;

static uint32_t current_process;
static uint32_t next_process;

static proc_t delay_process;

/*allocation counter is a 32 bit flag, where 1 means available 0 means occupied*/
static uint32_t allocation_counter;

static void sleep_function(void){
    while(1){
        ;;
    }
}

void scheduler_init(void){
    
    /*Initialize delay process.*/
    delay_process.stack_pointer = &heap_start + STACK_FRAME;
    delay_process.priority = 0xFF;
    delay_process.function = &sleep_function;
    allocation_counter = 0xFFFFFFFF;
    scheduler_create_process(&delay_process);
    
    current_process = 0;
    next_process = 0;
}

bool scheduler_create_process(proc_t *process){
    if(allocation_counter != 0){
        uint32_t stack_place = count_1s(allocation_counter ^ (allocation_counter - 1));
        process->index = stack_place - 1;
        process->stack_pointer = (uint32_t *)&heap_start + (process->index+1)*STACK_FRAME;
        allocation_counter &= ~(1 << process->index);
        process_table[process->index]=process;

        /* Set stack frame.*/
        process->stack_pointer--;
        *process->stack_pointer = ((uint32_t)1 << 24); /*PSR*/
        process->stack_pointer--;
        *process->stack_pointer = (uint32_t)process->function; /*PC*/
        process->stack_pointer--;
        *process->stack_pointer = 0xFFFFFFFD; /*LR*/
        process->stack_pointer--;
        *process->stack_pointer = 0x00000000; /*R12*/
        process->stack_pointer--;
        *process->stack_pointer = 0x00000000; /*R3*/
        process->stack_pointer--;
        *process->stack_pointer = 0x00000000; /*R2*/
        process->stack_pointer--;
        *process->stack_pointer = 0x00000000; /*R1*/
        process->stack_pointer--;
        *process->stack_pointer = 0x00000000; /*R0*/
        process->stack_pointer--;
        *process->stack_pointer = 0x00000000; /*R4*/
        process->stack_pointer--;
        *process->stack_pointer = 0x00000000; /*R5*/
        process->stack_pointer--;
        *process->stack_pointer = 0x00000000; /*R6*/
        process->stack_pointer--;
        *process->stack_pointer = 0x00000000; /*R7*/
        process->stack_pointer--;
        *process->stack_pointer = 0x00000000; /*R8*/
        process->stack_pointer--;
        *process->stack_pointer = 0x00000000; /*R9*/
        process->stack_pointer--;
        *process->stack_pointer = 0x00000000; /*R10*/
        process->stack_pointer--;
        *process->stack_pointer = 0x00000000; /*R11*/
        
        return true;
    } else{
        return false;
    }
}

bool scheduler_delete_process(proc_t *process){
    if(process_table[process->index] != process){
        return false;
    }
    if(process->index == 0){
        return false;
    }
    process_table[process->index] = 0;
    allocation_counter |= (1 << process->index);
    return true;
}

void scheduler_schedule(void){
    next_process = current_process;
    for(uint32_t i=1; i<32; i++){
        uint32_t index = (current_process + i) & 0x1F;
        if(process_table[index] != 0){
            next_process = index;
            return;
        }
    }
}

void scheduler_start(void){
    scheduler_schedule();
    current_process = 0xFFFFFFFF;
    *(volatile uint32_t *)(0xE000ED00 + 0x04) |= ((uint32_t)1 << 28);
}

void SysTick_Handler(void){
    scheduler_schedule();
    if (next_process != current_process){
        /*Enable pendSV interrupt.*/
        *(volatile uint32_t *)(0xE000ED00 + 0x04) |= ((uint32_t)1 << 28);
    }
}

__attribute__((naked)) void PendSV_Handler(void){
    __asm__ volatile (
        /* push registers to current PSP stack.*/
        "LDR r2, =process_table \n"
        "LDR r3, =current_process\n"
        "LDR r12, [r3]\n"
        "CMP r12, #0\n"
        "BLT FIRST_TASK\n"
        "MRS r1, PSP\n"
        "STR r4, [r1, #-4]!\n"
        "STR r5, [r1, #-4]!\n"
        "STR r6, [r1, #-4]!\n"
        "STR r7, [r1, #-4]!\n"
        "STR r8, [r1, #-4]!\n"
        "STR r9, [r1, #-4]!\n"
        "STR r10, [r1, #-4]!\n"
        "STR r11, [r1, #-4]!\n"
        /* Store PSP to stack pointer field in process struct.*/
        "LDR r4, [r3]\n"
        "LSL r6, r4, #2\n"
        "LDR r8, [r2, r6]\n"
        "STR r1, [r8]\n"
        /* Change current process to next process.*/
        "FIRST_TASK:\n"
        "LDR r5, =next_process\n"
        "LDR r4, [r5]\n"
        "STR r4, [r3]\n"
        /* Switch to other process.*/
        "LSL r6, r4, #2\n"
        "LDR r8, [r2, r6]\n"
        "LDR r1, [r8]\n"
        /* Pop registers into new pointer.*/
        "LDR r11, [r1], #4\n"
        "LDR r10, [r1], #4\n"
        "LDR r9, [r1], #4\n"
        "LDR r8, [r1], #4\n"
        "LDR r7, [r1], #4\n"
        "LDR r6, [r1], #4\n"
        "LDR r5, [r1], #4\n"
        "LDR r4, [r1], #4\n"
        "MSR PSP, r1\n"
        "LDR LR, =0xFFFFFFFD\n"
        "BX LR\n"
    );
}
