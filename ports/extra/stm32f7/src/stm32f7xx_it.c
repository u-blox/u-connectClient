/**
  ******************************************************************************
  * @file    stm32f7xx_it.c
  * @brief   Main Interrupt Service Routines for STM32F7xx.
  *          Provides exception handlers and basic peripheral interrupt handlers.
  *
  * This file is adapted for ucx-matter-app (no Ethernet, uses FreeRTOS directly)
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stm32f7xx_it.h"
#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS port layer - needed for xPortSysTickHandler */
extern void xPortSysTickHandler(void);

/* ucxclient UART port handlers (u_port_uart_stm32f7.c) */
extern void uPortUart_IRQHandler(void);
extern void uPortUartDma_IRQHandler(void);
/* Console UART RX handler (main_stm32.c) */
extern void exampleConsoleUart_IRQHandler(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M7 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Debug Monitor exception.
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * Note: FreeRTOS uses xPortSysTickHandler - configured via FreeRTOSConfig.h
  */
void SysTick_Handler(void)
{
    HAL_IncTick();
#if (INCLUDE_xTaskGetSchedulerState == 1)
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
#endif
        xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState == 1)
    }
#endif
}

/******************************************************************************/
/*                 STM32F7xx Peripherals Interrupt Handlers                   */
/******************************************************************************/

/**
  * @brief This function handles USART1 global interrupt (u-blox module).
  */
void USART1_IRQHandler(void)
{
  uPortUart_IRQHandler();
}

/**
  * @brief This function handles DMA2 Stream 2 global interrupt (USART1 RX DMA).
  */
void DMA2_Stream2_IRQHandler(void)
{
  uPortUartDma_IRQHandler();
}

/**
  * @brief This function handles USART3 global interrupt (console/ST-LINK VCP).
  *
  * Feeds the interrupt-driven RX ring buffer in main_stm32.c so
  * exampleConsoleUartRead() never overruns during sustained/binary transfers
  * (e.g. XMODEM via uart_bridge_example).
  */
void USART3_IRQHandler(void)
{
  exampleConsoleUart_IRQHandler();
}