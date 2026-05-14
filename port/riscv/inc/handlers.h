#include <stdint.h>

void timer_handler(void);
void external_handler(void);
void ecall_handler(void);
void break_handler(void);
void invalid_instruction_handler(void);
void misaligned_load_handler(void);
void misaligned_store_handler(void);

void default_handler(void);
