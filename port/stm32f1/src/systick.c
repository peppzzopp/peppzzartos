#include "systick.h"
#include <stdint.h>

/* SYSTICK */
#define SYSTICK_BASE (0xE000E010)
#define STK_CTRL (*(volatile uint32_t *)(SYSTICK_BASE))
#define STK_LOAD (*(volatile uint32_t *)(SYSTICK_BASE + 0x4))
#define STK_VAL (*(volatile uint32_t *)(SYSTICK_BASE + 0x8))
#define STK_CALIB (*(volatile uint32_t *)(SYSTICK_BASE + 0xC))

void systick_enable(void){
    uint32_t ten_ms_HCLK_by_8 = STK_CALIB & ~0xFF000000;
    STK_CTRL = (uint32_t)0b011;
    STK_VAL = 0;
    uint32_t reload_value = ten_ms_HCLK_by_8;
    STK_LOAD = 999;
}
