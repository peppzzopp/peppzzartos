#ifndef INC_MUTEX_H
#define INC_MUTEX_H

#include <stdint.h>

/* Struct for Mutex and Semaphore */
/* API structure to access them */

typedef struct{
    uint8_t owner;
    uint8_t status;
    /*0 - free, 1 - owned*/
    uint32_t in_line;
    /*0 - not in line, 1 - in line.*/
}kernel_mutex_t;

kernel_mutex_t kernel_mutex_create(void);
void kernel_mutex_own(kernel_mutex_t *mutex);
void kernel_mutex_free(kernel_mutex_t *mutex);

#endif
