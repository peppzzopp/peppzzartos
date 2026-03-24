#ifndef INC_SCHEDULER_H
#define INC_SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct{
    uint32_t *stack_pointer;
    /*decreasing order of priority*/
    uint8_t priority;
    uint8_t index;
    void (*function)(void);
}proc_t;

void scheduler_init(void);
bool scheduler_create_process(proc_t *process);
bool scheduler_delete_process(proc_t *process);
void scheduler_schedule(void);
void scheduler_start(void);

#endif
