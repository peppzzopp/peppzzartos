#ifndef INC_KERNEL_INTERNAL_H
#define INC_KERNEL_INTERNAL_H

#include "task.h"

void kernel_ticks(void);
void kernel_yield(void);
void kernel_enter_critical(void);
void kernel_exit_critical(void);

#ifdef PEPPZZARTOS_TIMING_ENABLE
void kernel_timing_init(void);
#endif

#endif
