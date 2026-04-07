#include "kernel_internal.h"
#include "interrupts.h"

#ifdef PEPPZZARTOS_TIMING_ENABLE
typedef struct{
    volatile uint32_t CTRL;
    volatile uint32_t CYCCNT;
}dwt_struct;

#define DEMCR (*(volatile uint32_t *)0xE000EDFC)
#define DWT ((dwt_struct *)0xE0001000)

uint32_t tick_start = 0;
uint32_t tick_end = 0;
uint32_t context_switch_start = 0;
uint32_t context_switch_end = 0;
#endif

extern uint32_t current_task;
extern uint32_t next_task;
extern task_t task_table[];

void SysTick_Handler(void){
    #ifdef PEPPZZARTOS_TIMING_ENABLE
    tick_start = DWT->CYCCNT;
    #endif
    kernel_ticks();
    #ifdef PEPPZZARTOS_TIMING_ENABLE
    tick_end = DWT->CYCCNT;
    #endif
}

void kernel_yield(void){
    *(volatile uint32_t *)(0xE000ED00 + 0x04) |= ((uint32_t)1 << 28);
}

__attribute__((naked)) void kernel_enter_critical(void){
    __asm__ volatile(
        "MOV r0, #1\n"
        "MSR PRIMASK, r0\n"
        "BX LR\n"
    );
}

__attribute__((naked)) void kernel_exit_critical(void){
    __asm__ volatile(
        "MOV r0, #0\n"
        "MSR PRIMASK, r0\n"
        "BX LR\n"
    );
}

__attribute__((naked)) void PendSV_Handler(void){
    __asm__ volatile (
        #ifdef PEPPZZARTOS_TIMING_ENABLE
        /*Time measurement for start of context_switch.*/
        "PUSH {r0, r1}\n"
        "LDR r0, =context_switch_start\n"
        "LDR r1, =0xE0001004\n"
        "LDR r1, [r1]\n"
        "STR r1, [r0]\n"
        "POP {r0, r1}\n"
        #endif
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
        /* Pop registers from new pointer.*/
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
        #ifdef PEPPZZARTOS_TIMING_ENABLE
        /*Time measurement for end of context_switch.*/
        "PUSH {r0, r1}\n"
        "LDR r0, =context_switch_end\n"
        "LDR r1, =0xE0001004\n"
        "LDR r1, [r1]\n"
        "STR r1, [r0]\n"
        "POP {r0, r1}\n"
        #endif
        "BX LR\n"
    );
}

#ifdef PEPPZZARTOS_TIMING_ENABLE
void kernel_timing_init(void){
    DEMCR |= ((uint32_t)1 << 24);
    DWT->CTRL |= (uint32_t)1;
    DWT->CYCCNT = 0;
}
#endif
