#ifndef INC_TASK_H
#define INC_TASK_H

#include <stdint.h>

typedef struct{
    uint32_t *stack_pointer;
    uint8_t priority;
    uint8_t index;
    void (*function)(void);
}task_t;

bool task_create();
bool task_delete();

#endif
