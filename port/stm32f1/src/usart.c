#include "interrupts.h"
#include "usart.h"

#define BUFFER_SIZE (64)
#define BUFFER_MASK ((uint8_t)(BUFFER_SIZE - 1))

#define NVIC_ISER1 (*(uint32_t *)(0xE000E100 + 0x4))

static volatile uint8_t transmit_buffer[BUFFER_SIZE] = {0};
static volatile uint8_t transmit_write_index;
static volatile uint8_t transmit_read_index;

static volatile uint8_t recieve_buffer[BUFFER_SIZE] = {0};
static volatile uint8_t recieve_write_index;
static volatile uint8_t recieve_read_index;

void usart_enable(void){
    RCC_APB1 |= ((uint32_t)1 << 17);
    uint32_t control_bits = 0;
    control_bits |= (1<<13) | (1<<3) | (1<<7) | (1<<5) | (1<<2);
    USART2->CR1 = control_bits;
    USART2->BRR = (0x4 << 4) | (0x5);
    
    NVIC_ISER1 |= ((uint32_t)1 << 6);

    transmit_write_index = 0;
    transmit_read_index = 0;
    recieve_write_index = 0;
    recieve_read_index = 0;
}

static bool usart_write_byte(uint8_t data){
    /*Return False if full*/
    if(((transmit_write_index + 1)&BUFFER_MASK) == transmit_read_index){
        return false;
    }
    transmit_buffer[transmit_write_index] = data;
    transmit_write_index = (transmit_write_index+1)&BUFFER_MASK;
    return true;
}

uint32_t usart_write(uint8_t *data, uint32_t length){
    uint32_t count=0;
    for(uint32_t i=0; i<length; i++){
        if(usart_write_byte(data[i])){
            count++;
        } else{
            USART2->CR1 |= ((uint32_t)1 << 7);
            return count;
        }
    }
    USART2->CR1 |= ((uint32_t)1 << 7);
    return count;
}

static bool usart_read_byte(uint8_t *data){
    /*Return False if empty*/
    if(recieve_write_index == recieve_read_index){
        return false;
    }
    *data = recieve_buffer[recieve_read_index];
    recieve_read_index=(recieve_read_index+1)&BUFFER_MASK;
    return true;
}

uint32_t usart_read(uint8_t *data, uint32_t length){
    uint32_t count=0;
    for(uint32_t i=0; i<length; i++){
        if(usart_read_byte(data+i)){
            count++;
        } else{
            return count;
        }
    }
    return count;
}

bool usart_data_available(void){
    return (recieve_read_index != recieve_write_index);
}

void USART2_Handler(void){
    uint32_t status_register = USART2->SR;
    if(status_register & (1<<5)){
        if(((recieve_write_index + 1)&BUFFER_MASK) != recieve_read_index){
            recieve_buffer[recieve_write_index] = USART2->DR;
            recieve_write_index = (recieve_write_index + 1)&BUFFER_MASK;
        } else{
            (void)USART2->DR;
        }
    }
    if(status_register & (1<<7)){
        if(transmit_read_index != transmit_write_index){
            USART2->DR = (uint32_t)transmit_buffer[transmit_read_index];
            transmit_read_index=(transmit_read_index+1)&BUFFER_MASK;
        } else{
            USART2->CR1 &= ~((uint32_t)1 << 7);
        }
    }
}
