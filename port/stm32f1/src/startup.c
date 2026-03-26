#include <stdint.h>
#include "interrupts.h"

#define SRAM_START  0x20000000
#define SRAM_SIZE  (20 * 1024)
#define SRAM_END  (SRAM_START + SRAM_SIZE)

#define STACK_START  SRAM_END

extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

extern int main(void);

uint32_t vector_table[76] __attribute__ ((section(".isr_vector"))) = {
    (uint32_t)STACK_START,
    (uint32_t)&Reset_Handler,
    (uint32_t)&NMI_Handler,
    (uint32_t)&HardFault_Handler,
    (uint32_t)&MemManage_Handler,
    (uint32_t)&BusFault_Handler,
    (uint32_t)&UsageFault_Handler,
    0,
    0,
    0,
    0,
    (uint32_t)&SVCall_Handler,
    (uint32_t)&DebugMonitor_Handler,
    0,
    (uint32_t)&PendSV_Handler,
    (uint32_t)&SysTick_Handler,
    (uint32_t)&WWDG_Handler,
    (uint32_t)&PVD_Handler,
    (uint32_t)&TAMPER_Handler,
    (uint32_t)&RTC_Handler,
    (uint32_t)&FLASH_Handler,
    (uint32_t)&RCC_Handler,
    (uint32_t)&EXTI0_Handler,
    (uint32_t)&EXTI1_Handler,
    (uint32_t)&EXTI2_Handler,
    (uint32_t)&EXTI3_Handler,
    (uint32_t)&EXTI4_Handler,
    (uint32_t)&DMA1_C1_Handler,
    (uint32_t)&DMA1_C2_Handler,
    (uint32_t)&DMA1_C3_Handler,
    (uint32_t)&DMA1_C4_Handler,
    (uint32_t)&DMA1_C5_Handler,
    (uint32_t)&DMA1_C6_Handler,
    (uint32_t)&DMA1_C7_Handler,
    (uint32_t)&ADC1_2_Handler,
    (uint32_t)&USB_HP_CAN_TX_Handler,
    (uint32_t)&USB_LP_CAN_RX0_Handler,
    (uint32_t)&CAN_RX1_Handler,
    (uint32_t)&CAN_SCE_Handler,
    (uint32_t)&EXTI9_5_Handler,
    (uint32_t)&TIM1_BRK_Handler,
    (uint32_t)&TIM1_UP_Handler,
    (uint32_t)&TIM1_TRG_COM_Handler,
    (uint32_t)&TIM1_CC_Handler,
    (uint32_t)&TIM2_Handler,
    (uint32_t)&TIM3_Handler,
    (uint32_t)&TIM4_Handler,
    (uint32_t)&I2C1_EV_Handler,
    (uint32_t)&I2C1_ER_Handler,
    (uint32_t)&I2C2_EV_Handler,
    (uint32_t)&I2C2_ER_Handler,
    (uint32_t)&SPI1_Handler,
    (uint32_t)&SPI2_Handler,
    (uint32_t)&USART1_Handler,
    (uint32_t)&USART2_Handler,
    (uint32_t)&USART3_Handler,
    (uint32_t)&EXTI15_10_Handler,
    (uint32_t)&RTCAlarm_Handler,
    (uint32_t)&USBWakeup_Handler,
    (uint32_t)&TIM8_BRK_Handler,
    (uint32_t)&TIM8_UP_Handler,
    (uint32_t)&TIM8_TRG_COM_Handler,
    (uint32_t)&TIM8_CC_Handler,
    (uint32_t)&ADC3_Handler,
    (uint32_t)&FSMC_Handler,
    (uint32_t)&SDIO_Handler,
    (uint32_t)&TIM5_Handler,
    (uint32_t)&SPI3_Handler,
    (uint32_t)&UART4_Handler,
    (uint32_t)&UART5_Handler,
    (uint32_t)&TIM6_Handler,
    (uint32_t)&TIM7_Handler,
    (uint32_t)&DMA2_Channel1_Handler,
    (uint32_t)&DMA2_Channel2_Handler,
    (uint32_t)&DMA2_Channel3_Handler,
    (uint32_t)&DMA2_Channel4_5_Handler
};

void Default_Handler(void){
    while(1);
}

void Reset_Handler(void){
    uint32_t size_of_data = (uint32_t)&_edata - (uint32_t)&_sdata;

    uint8_t *pDst = (uint8_t *)&_sdata;
    uint8_t *pSrc = (uint8_t *)&_etext;

    for(uint32_t i=0; i<size_of_data; i++){
        *pDst++ = *pSrc++;
    }

    size_of_data = (uint32_t)&_ebss - (uint32_t)&_sbss;
    pDst = (uint8_t *)&_sbss;
    for(uint32_t i=0; i<size_of_data; i++){
        *pDst++ = 0;
    }
    main();
}


void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void) __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVCall_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DebugMonitor_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));
void WWDG_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PVD_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TAMPER_Handler(void) __attribute__((weak, alias("Default_Handler")));
void RTC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void FLASH_Handler(void) __attribute__((weak, alias("Default_Handler")));
void RCC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI2_Handler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI3_Handler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI4_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_C1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_C2_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_C3_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_C4_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_C5_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_C6_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_C7_Handler(void) __attribute__((weak, alias("Default_Handler")));
void ADC1_2_Handler(void) __attribute__((weak, alias("Default_Handler")));
void USB_HP_CAN_TX_Handler(void) __attribute__((weak, alias("Default_Handler")));
void USB_LP_CAN_RX0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void CAN_RX1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void CAN_SCE_Handler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI9_5_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_BRK_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_UP_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_TRG_COM_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_CC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM2_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM3_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM4_Handler(void) __attribute__((weak, alias("Default_Handler")));
void I2C1_EV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void I2C1_ER_Handler(void) __attribute__((weak, alias("Default_Handler")));
void I2C2_EV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void I2C2_ER_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SPI1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SPI2_Handler(void) __attribute__((weak, alias("Default_Handler")));
void USART1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void USART2_Handler(void) __attribute__((weak, alias("Default_Handler")));
void USART3_Handler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI15_10_Handler(void) __attribute__((weak, alias("Default_Handler")));
void RTCAlarm_Handler(void) __attribute__((weak, alias("Default_Handler")));
void USBWakeup_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM8_BRK_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM8_UP_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM8_TRG_COM_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM8_CC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void ADC3_Handler(void) __attribute__((weak, alias("Default_Handler")));
void FSMC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SDIO_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM5_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SPI3_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UART4_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UART5_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM6_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM7_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Channel1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Channel2_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Channel3_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Channel4_5_Handler(void) __attribute__((weak, alias("Default_Handler")));

