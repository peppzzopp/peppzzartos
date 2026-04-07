#ifndef INC_SEMAPHORE_H
#define INC_SEMAPHORE_H

#include <stdint.h>

typedef struct{
    uint8_t count;
    uint8_t max_count;
    uint8_t last_task;
    uint32_t in_line;
}kernel_semaphore_t;

kernel_semaphore_t kernel_semaphore_create(uint8_t count);
void kernel_semaphore_wait(kernel_semaphore_t *semaphore);
void kernel_semaphore_signal(kernel_semaphore_t *semaphore);

#endif
