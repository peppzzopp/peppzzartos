#include "kernel_internal.h"
#include "handlers.h"

#ifdef PEPPZZARTOS_TIMING_ENABLE
extern uint32_t trap_start;
extern uint32_t trap_end;
extern uint32_t tick_start;
extern uint32_t tick_end;
extern uint32_t context_switch_start;
extern uint32_t context_switch_end;
#endif


extern uint32_t current_task;
extern uint32_t next_task;
extern task_t task_table[];
extern uint32_t in_trap;

uint32_t one_millisec = 0x6977;

uint32_t last_stack_pointer;

__attribute__((naked)) void timer_reset(void){
    __asm__ volatile(
        "li t0, 0x30000100\n"
        "addi t1, t0, 0x4\n"
        "la t6, one_millisec\n"
        "lw t6, 0x0(t6)\n"
        "lw t2, 0x0(t0)\n"
        "lw t3, 0x0(t1)\n"
        "add t4, t2, t6\n"
        "sltu t5, t4, t2\n"
        "add t3, t3, t5\n"
        "sw t4, 0x0(t0)\n"
        "sw t3, 0x0(t1)\n"
        "ret\n"
    );
}

__attribute__((naked)) void timer_handler(void){
    __asm__ volatile(
        
        "la t1, last_stack_pointer\n"
        "sw sp, 0x0(t1)\n"
        
        "call timer_reset\n"

        "call kernel_ticks\n"
        
        "la t0, last_stack_pointer\n"
        "sw x0, 0x0(t0)\n"

        "la t0, in_trap\n"
        "sw x0, 0x0(t0)\n"

        "lw x1, 0x0(sp)\n"
        "lw x3, 0x4(sp)\n"
        "lw x4, 0x8(sp)\n"
        "lw x5, 0xc(sp)\n"
        "lw x6, 0x10(sp)\n"
        "lw x7, 0x14(sp)\n"
        "lw x8, 0x18(sp)\n"
        "lw x9, 0x1c(sp)\n"
        "lw x10, 0x20(sp)\n"
        "lw x11, 0x24(sp)\n"
        "lw x12, 0x28(sp)\n"
        "lw x13, 0x2c(sp)\n"
        "lw x14, 0x30(sp)\n"
        "lw x15, 0x34(sp)\n"
        "lw x16, 0x38(sp)\n"
        "lw x17, 0x3c(sp)\n"
        "lw x18, 0x40(sp)\n"
        "lw x19, 0x44(sp)\n"
        "lw x20, 0x48(sp)\n"
        "lw x21, 0x4c(sp)\n"
        "lw x22, 0x50(sp)\n"
        "lw x23, 0x54(sp)\n"
        "lw x24, 0x58(sp)\n"
        "lw x25, 0x5c(sp)\n"
        "lw x26, 0x60(sp)\n"
        "lw x27, 0x64(sp)\n"
        "lw x28, 0x68(sp)\n"
        "lw x29, 0x6c(sp)\n"
        "lw x30, 0x70(sp)\n"
        "lw x31, 0x74(sp)\n"

        "addi sp, sp, 128\n"
        
        #ifdef PEPPZZARTOS_TIMING_ENABLE
        "addi sp, sp, -0x8\n"
        "sw t0, 0x4(sp)\n"
        "sw t1, 0x0(sp)\n"
        "la t0, trap_end\n"
        "csrr t1, mcycle\n"
        "sw t1, 0x0(t0)\n"
        "lw t1, 0x0(sp)\n"
        "lw t0, 0x4(sp)\n"
        "addi sp, sp, 0x8\n"
        #endif

        "mret"
    );
}

void kernel_init_task_stack(task_t *task){
    task->stack_pointer--;
    *task->stack_pointer = 0x00000000;
    task->stack_pointer--;
    *task->stack_pointer = (uint32_t)task->task_function;
    for(uint32_t i=0; i<30; i++){
        task->stack_pointer--;
        *task->stack_pointer = 0x00000000;
    }
}

__attribute__((naked)) void kernel_yield(void){
    __asm__ volatile(
        "ecall\n"
        "ret"
    );
}

__attribute__((naked)) void kernel_enter_critical(void){
    __asm__ volatile(
        "csrci mstatus, 0x8\n"
        "ret"
    );
}

__attribute__((naked)) void kernel_exit_critical(void){
    __asm__ volatile(
        "csrsi mstatus, 0x8\n"
        "ret"
    );
}

__attribute__((naked)) void ecall_handler(void){
    __asm__ volatile(
        
        /*Check for first task*/
        "la t0, current_task\n"
        "lw t1, 0x0(t0)\n"
        "blt t1, x0, first_task_now\n"

        /*Check if from timing handler*/
        "la t0, last_stack_pointer\n"
        "lw t1, 0x0(t0)\n"
        "bne t1, x0, ecall_from_timing\n"
        /*Now assuming direct ecall*/
        "lw t0, 0x78(sp)\n"
        "addi t0, t0, 0x4\n"
        "sw t0, 0x78(sp)\n"
        "j context_switch_now\n"
        /*timing handler path*/
        "ecall_from_timing:\n"
        "add sp, t1, x0\n"
        "sw x0, 0x0(t0)\n"

        "context_switch_now:\n"
        /*Store stack pointer in current task*/
        "la t0, task_table\n"
        "la t1, current_task\n"
        "lw t2, 0x0(t1)\n"
        "slli t2, t2, 0x2\n"
        "add t2, t2, t0\n"
        "lw t2, 0x0(t2)\n"
        "sw sp, 0x0(t2)\n"
        /*Get stack pointer from next task*/
        "first_task_now:\n"
        "la t0, task_table\n"
        "la t1, current_task\n"
        "la t3, next_task\n"
        "lw t4, 0x0(t3)\n"
        "sw t4, 0x0(t1)\n"
        "slli t4, t4, 0x2\n"
        "add t4, t4, t0\n"
        "lw t4, 0x0(t4)\n"
        "lw sp, 0x0(t4)\n"
        "lw t0, 0x78(sp)\n"
        "csrw mepc, t0\n"

        "la t0, in_trap\n"
        "sw x0, 0x0(t0)\n"

        "li t0, 0x80\n"
        "csrs mstatus, t0\n"

        "lw x1, 0x0(sp)\n"
        "lw x3, 0x4(sp)\n"
        "lw x4, 0x8(sp)\n"
        "lw x5, 0xc(sp)\n"
        "lw x6, 0x10(sp)\n"
        "lw x7, 0x14(sp)\n"
        "lw x8, 0x18(sp)\n"
        "lw x9, 0x1c(sp)\n"
        "lw x10, 0x20(sp)\n"
        "lw x11, 0x24(sp)\n"
        "lw x12, 0x28(sp)\n"
        "lw x13, 0x2c(sp)\n"
        "lw x14, 0x30(sp)\n"
        "lw x15, 0x34(sp)\n"
        "lw x16, 0x38(sp)\n"
        "lw x17, 0x3c(sp)\n"
        "lw x18, 0x40(sp)\n"
        "lw x19, 0x44(sp)\n"
        "lw x20, 0x48(sp)\n"
        "lw x21, 0x4c(sp)\n"
        "lw x22, 0x50(sp)\n"
        "lw x23, 0x54(sp)\n"
        "lw x24, 0x58(sp)\n"
        "lw x25, 0x5c(sp)\n"
        "lw x26, 0x60(sp)\n"
        "lw x27, 0x64(sp)\n"
        "lw x28, 0x68(sp)\n"
        "lw x29, 0x6c(sp)\n"
        "lw x30, 0x70(sp)\n"
        "lw x31, 0x74(sp)\n"
        "addi sp, sp, 128\n"
        
        #ifdef PEPPZZARTOS_TIMING_ENABLE
        "addi sp, sp, -0x8\n"
        "sw t0, 0x4(sp)\n"
        "sw t1, 0x0(sp)\n"
        "la t0, trap_end\n"
        "csrr t1, mcycle\n"
        "sw t1, 0x0(t0)\n"
        "lw t1, 0x0(sp)\n"
        "lw t0, 0x4(sp)\n"
        "addi sp, sp, 0x8\n"
        #endif

        "mret"
    );
}

#ifdef PEPPZZARTOS_TIMING_ENABLE
void kernel_timing_init(void){
    /*Nothing to do here.*/
}
#endif
