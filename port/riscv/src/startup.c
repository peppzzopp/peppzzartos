#include "handlers.h"
#include <stdint.h>

#ifdef PEPPZZARTOS_TIMING_ENABLE
uint32_t trap_start = 0;
uint32_t trap_end = 0;
uint32_t tick_start = 0;
uint32_t tick_end = 0;
uint32_t context_switch_start = 0;
uint32_t context_switch_end = 0;
#endif


extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

extern int main(void);

__attribute__((interrupt("machine"))) void default_handler(void){
    while(1){
        ;;
    }
}

void timer_handler(void) __attribute__((weak, alias("default_handler")));
void external_handler(void) __attribute__((weak, alias("default_handler")));
void ecall_handler(void) __attribute__((weak, alias("default_handler")));
void break_handler(void) __attribute__((weak, alias("default_handler")));
void invalid_instruction_handler(void) __attribute__((weak, alias("default_handler")));
void misaligned_load_handler(void) __attribute__((weak, alias("default_handler")));
void misaligned_store_handler(void) __attribute__((weak, alias("default_handler")));


uint32_t in_trap = 0x00000000;


__attribute__((naked)) uint32_t trap_decode(void){
    __asm__ volatile(
        "addi sp, sp, -0x8\n"
        "sw t0, 0x4(sp)\n"
        "sw t1, 0x0(sp)\n"
        "la t0, in_trap\n"
        "lw t1, 0x0(t0)\n"
        "bne t1, x0, in_a_trap_handler\n"
        "li t1, 0xFFFFFFFF\n"
        "sw t1, 0x0(t0)\n"
       
        #ifdef PEPPZZARTOS_TIMING_ENABLE
        "la t0, trap_start\n"
        "csrr t1, mcycle\n"
        "sw t1, 0x0(t0)\n"
        #endif

        "lw t1, 0x0(sp)\n"
        "lw t0, 0x4(sp)\n"
        "addi sp, sp, 0x8\n"
        "addi sp, sp, -128\n"
        "sw x1, 0x0(sp)\n"
        "sw x3, 0x4(sp)\n"
        "sw x4, 0x8(sp)\n"
        "sw x5, 0xc(sp)\n"
        "sw x6, 0x10(sp)\n"
        "sw x7, 0x14(sp)\n"
        "sw x8, 0x18(sp)\n"
        "sw x9, 0x1c(sp)\n"
        "sw x10, 0x20(sp)\n"
        "sw x11, 0x24(sp)\n"
        "sw x12, 0x28(sp)\n"
        "sw x13, 0x2c(sp)\n"
        "sw x14, 0x30(sp)\n"
        "sw x15, 0x34(sp)\n"
        "sw x16, 0x38(sp)\n"
        "sw x17, 0x3c(sp)\n"
        "sw x18, 0x40(sp)\n"
        "sw x19, 0x44(sp)\n"
        "sw x20, 0x48(sp)\n"
        "sw x21, 0x4c(sp)\n"
        "sw x22, 0x50(sp)\n"
        "sw x23, 0x54(sp)\n"
        "sw x24, 0x58(sp)\n"
        "sw x25, 0x5c(sp)\n"
        "sw x26, 0x60(sp)\n"
        "sw x27, 0x64(sp)\n"
        "sw x28, 0x68(sp)\n"
        "sw x29, 0x6c(sp)\n"
        "sw x30, 0x70(sp)\n"
        "sw x31, 0x74(sp)\n"
        "csrr t0, mepc\n"
        "sw t0, 0x78(sp)\n"
        "j branch_now\n"
        "in_a_trap_handler:\n"
        "lw t1, 0x0(sp)\n"
        "lw t0, 0x4(sp)\n"
        "addi sp, sp, 0x8\n"

        "branch_now:\n"
        "csrr t1, mcause\n"
        "xori t2, t1, 0x2\n"
        "beq t2, x0, invalid_instruction_handler\n"
        "xori t2, t1, 0x3\n"
        "beq t2, x0, break_handler\n"
        "xori t2, t1, 0x4\n"
        "beq t2, x0, misaligned_load_handler\n"
        "xori t2, t1, 0x6\n"
        "beq t2, x0, misaligned_store_handler\n"
        "xori t2, t1, 0xB\n"
        "beq t2, x0, ecall_handler\n"
        "lui t3, 0x80000\n"
        "addi t3, t3, 0x7\n"
        "xor t2, t1, t3\n"
        "beq t2, x0, timer_handler\n"
        "addi t3, t3, 0x4\n"
        "xor t2, t1, t3\n"
        "beq t2, x0, external_handler\n"
        "j default_handler\n"
    );
}


void init_machine_interrupts(void) {
    __asm__ volatile(
        "csrw mtvec, %0\n"
        "li t0, 0x80\n"
        "csrs mie, t0\n"
        "li t0, 0x8\n"
        "csrs mstatus, t0\n"
        : /* No outputs */
        : "r" (trap_decode)
        : "t0"
    );
}

void start_up(void);
__attribute__((naked, section(".text.entry"))) void _stack_setup(void){
    __asm__ volatile(
        "la sp, _stack\n"
        "j start_up\n"
    );
}

void start_up(void){
    uint32_t size_of_data = (uint32_t)&_edata - (uint32_t)&_sdata;

    uint8_t *pDst = (uint8_t *)&_sdata;
    uint8_t *pSrc = (uint8_t *)&_etext;

    for(uint32_t i=0; i<size_of_data; i++){
        *pDst++ = *pSrc++;
    }

    size_of_data = (uint32_t)&_ebss - (uint32_t)&_sbss;
    pDst = (uint8_t *)&_sbss;
    for(uint32_t i=0; i<size_of_data; i++){
        *pDst++ = 0;
    }
    init_machine_interrupts();
    main();
}
