#ifndef INC_TASK_H
#define INC_TASK_H
#include <stdint.h>

typedef struct{
    uint32_t *stack_pointer;
    uint8_t state_of_task;
    /*0 - not ready, 1 - ready*/
    uint8_t delay_timer;
    void (*task_function)(void);
}task_t;

#endif
