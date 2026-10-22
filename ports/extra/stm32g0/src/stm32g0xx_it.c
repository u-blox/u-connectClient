/* Interrupt handlers for the NUCLEO-G071RB. */

#include "stm32g0xx_hal.h"
#include "cmsis_os.h"
#include <stdio.h>

extern void uPortUart_IRQHandler(void);
extern void exampleConsoleUart_IRQHandler(void);

void NMI_Handler(void);
void HardFault_Handler(void);
void SysTick_Handler(void);
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    printf("\r\nFATAL: HardFault\r\n");
    while (1);
}

/* SysTick drives both the HAL tick (also before the scheduler runs) and FreeRTOS. */
void SysTick_Handler(void)
{
    HAL_IncTick();
    osSystickHandler();
}

/* u-blox module (USART1, PA9/PA10) */
void USART1_IRQHandler(void)
{
    uPortUart_IRQHandler();
}

/* Console (USART2, PA2/PA3 = ST-LINK VCP) */
void USART2_IRQHandler(void)
{
    exampleConsoleUart_IRQHandler();
}
