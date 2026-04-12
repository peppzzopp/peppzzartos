#ifndef INC_TASK_H
#define INC_TASK_H
#include <stdint.h>

typedef struct{
    uint32_t *stack_pointer;
    volatile uint32_t delay_timer;
    void (*task_function)(void);
}task_t;

#endif
