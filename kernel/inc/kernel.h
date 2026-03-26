#ifndef INC_KERNEL_H
#define INC_KERNEL_H

#include <task.h>
#include <stdbool.h>

void kernel_init(void);
void kernel_start(void);
int32_t kernel_task_add(void (*function)(void));
bool kernel_task_delete(int32_t index);
void kernel_task_delay(uint32_t delay);

#endif
