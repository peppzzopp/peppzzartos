#include "kernel_internal.h"
#include "interrupts.h"

extern uint32_t current_task;
extern uint32_t next_task;
extern task_t task_table[];

void SysTick_Handler(void){
    kernel_ticks();
}

void kernel_yield(void){
    *(volatile uint32_t *)(0xE000ED00 + 0x04) |= ((uint32_t)1 << 28);
}

__attribute__((naked)) void PendSV_Handler(void){
    __asm__ volatile (
        /* push registers to current PSP stack.*/
        "LDR r2, =task_table \n"
        "LDR r3, =current_task\n"
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
        "LDR r5, =next_task\n"
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
