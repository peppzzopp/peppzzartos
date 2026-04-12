#ifndef INC_USART_H
#define INC_USART_H

#include <stdint.h>
#include <stdbool.h>

/*RCC Enable*/
#define RCC_BASE (0x40021000)
#define APB2_OFFSET (0x18)
#define APB1_OFFSET (0x1C)
#define RCC_APB1 (*(volatile uint32_t *)(RCC_BASE + APB1_OFFSET))
#define RCC_APB2 (*(volatile uint32_t *)(RCC_BASE + APB2_OFFSET))

/*CONFIG Registers*/
#define USART1_BASE (0x40013800)
#define USART2_BASE (0x40004400)
#define USART3_BASE (0x40004800)
#define UART4_BASE (0x40004C00)
#define UART5_BASE (0x40005000)

typedef struct{
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
}USART_t;

#define USART1 ((USART_t *)(USART1_BASE))
#define USART2 ((USART_t *)(USART2_BASE))
#define USART3 ((USART_t *)(USART3_BASE))
#define UART4 ((USART_t *)(UART4_BASE))
#define UART5 ((USART_t *)(UART5_BASE))

void usart_enable(void);
uint32_t usart_write(uint8_t *data, uint32_t length);
uint32_t usart_read(uint8_t *data, uint32_t length);
void usart_clear_send(void);
void usart_clear_recieve(void);
bool usart_data_available(void);

#endif
