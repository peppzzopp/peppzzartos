#include "gpio.h"

/* RCC */
#define RCC_BASE (0x40021000)
#define APB2_OFFSET (0x18)
#define RCC_APB2 (*(volatile uint32_t *)(RCC_BASE + APB2_OFFSET))

#define GPIO_BASE (0x40010800)
#define PORT_SIZE (0x400)

typedef struct{
    volatile uint32_t CRL;
    volatile uint32_t CRH;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
} gpio_t;

void gpio_enable(void){
    RCC_APB2 |= ((uint32_t)1 << 2);
}

void gpio_configure(GPIO_PORT port, uint8_t pin, uint8_t mode, uint8_t cnf){
    /*Get Port address*/
    gpio_t *port_pointer;
    port_pointer = (gpio_t *)((uint8_t *)GPIO_BASE + ((PORT_SIZE) * (uint32_t) port));
    uint32_t mode_cnf = ((mode & 0x3) | ((cnf & 0x3) << 2)) & 0x0000000F;
    if(pin < 8){
        mode_cnf = (mode_cnf << (pin*4));
        port_pointer->CRL &= ~((0xF)<<(pin * 4));
        port_pointer->CRL |= mode_cnf;
    } else{
        mode_cnf = (mode_cnf << ((pin - 8)*4));
        port_pointer->CRH &= ~((0xF)<<((pin - 8) * 4));
        port_pointer->CRH |= mode_cnf;
    }
}
void gpio_set_high(GPIO_PORT port, uint8_t pin){
    gpio_t *port_pointer;
    port_pointer = (gpio_t *)((uint8_t *)GPIO_BASE + ((PORT_SIZE) * (uint32_t) port));
    port_pointer->ODR |= ((uint32_t)1<<pin);
}
void gpio_set_low(GPIO_PORT port, uint8_t pin){
    gpio_t *port_pointer;
    port_pointer = (gpio_t *)((uint8_t *)GPIO_BASE + ((PORT_SIZE) * (uint32_t) port));
    port_pointer->ODR &= ~((uint32_t)1<<pin);
}
void gpio_toggle(GPIO_PORT port, uint8_t pin){
    gpio_t *port_pointer;
    port_pointer = (gpio_t *)((uint8_t *)GPIO_BASE + ((PORT_SIZE) * (uint32_t) port));
    port_pointer->ODR ^= ((uint32_t)1<<pin);
}
uint8_t gpio_read(GPIO_PORT port, uint8_t pin){
    gpio_t *port_pointer;
    port_pointer = (gpio_t *)((uint8_t *)GPIO_BASE + ((PORT_SIZE) * (uint32_t) port));
    uint32_t read = port_pointer->IDR;
    return (uint8_t)((read >> pin) & 0x1);
}
