#ifndef inc_gpio_h
#define inc_gpio_h

#include <stdint.h>

// struct gpio_ports{
//     uint32_t portA[256];
//     uint32_t portB[256];
//     uint32_t portC[256];
//     uint32_t portD[256];
//     uint32_t portE[256];
//     uint32_t portF[256];
//     uint32_t portG[256];
// };

typedef enum{
    portA,
    portB,
    portC,
    portD,
    portE,
    portF,
    portG,
} GPIO_PORT;

void gpio_enable(void);
void gpio_configure(GPIO_PORT port, uint8_t pin, uint8_t mode, uint8_t cnf);
void gpio_set_high(GPIO_PORT port, uint8_t pin);
void gpio_set_low(GPIO_PORT port, uint8_t pin);
void gpio_toggle(GPIO_PORT port, uint8_t pin);
uint8_t gpio_read(GPIO_PORT port, uint8_t pin);
#endif
