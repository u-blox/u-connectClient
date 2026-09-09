/*
 * Copyright 2025 u-blox
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @file
 * @brief STM32F779 UART port configuration
 *
 * UART Configuration:
 * -------------------
 * USART6 (u-blox module communication):
 *   - PG14: TX
 *   - PG9:  RX
 *   - PG13: CTS (when hardware flow control enabled)
 *   - PG12: RTS (when hardware flow control enabled)
 *   - Baud: Configurable (typically 115200)
 *   - Circular RX DMA: DMA2 Stream2 / Channel 5
 */

#ifndef U_PORT_UART_STM32F779_H
#define U_PORT_UART_STM32F779_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef U_PORT_UART_RX_BUFFER_SIZE
#define U_PORT_UART_RX_BUFFER_SIZE  (8192)
#endif

/** Structure representing a UART handle.
 */
typedef struct {
    UART_HandleTypeDef huart;
    DMA_HandleTypeDef hdmaRx;

    // __ALIGNED (CMSIS cmsis_compiler.h, pulled in via stm32f7xx_hal.h) forces
    // both this member's offset AND the whole struct's alignment to a D-cache
    // line boundary, regardless of the (opaque, HAL version-dependent) sizes
    // of the members above - no manual byte-counted padding needed. Using the
    // CMSIS macro instead of a raw __attribute__ keeps this portable to IAR
    // (__ICCARM__) as well as GCC/Clang/ARMCC.
    uint8_t rxBuffer[U_PORT_UART_RX_BUFFER_SIZE] __ALIGNED(32);

    uint32_t rxTotalRead;            
    volatile uint32_t rxWraps;       
    volatile bool rxResync;          
    volatile uint32_t errorCount;    
    volatile uint32_t overflowCount; 
    volatile uint32_t lastErrorCode; // HAL ErrorCode latched in ISR, printed later from task context
    bool isOpen;
} uPortUartHandle;


/* ----------------------------------------------------------------
 * COMPILE-TIME MACROS
 * -------------------------------------------------------------- */

/**
 * UART instance for u-blox module communication.
 * See file header for complete pin assignment documentation.
 */
#define U_PORT_UART_INSTANCE    USART6
#define U_PORT_UART_IRQn        USART6_IRQn
#define U_PORT_UART_IRQHandler  USART6_IRQHandler
#define U_PORT_UART_CLK_ENABLE  __HAL_RCC_USART6_CLK_ENABLE
#define U_PORT_UART_CLK_DISABLE __HAL_RCC_USART6_CLK_DISABLE

/**
 * RX DMA configuration. On STM32F7, USART6_RX maps to DMA2 Stream2 /
 * Channel 5 (fixed stream/channel mapping - no DMAMUX on F7).
 * RX uses circular DMA into a ring buffer so that no per-byte interrupts
 * are needed - required for reliable operation at high baud rates.
 *
 * NOTE: the ring buffer must be DMA-accessible. On F7, DTCM (0x20000000)
 * is reachable by DMA masters through the AHBS bus, but if the FreeRTOS
 * heap is placed elsewhere verify the region is DMA-reachable and the
 * MPU/cache settings match the invalidate-before-read strategy used here.
 */
#define U_PORT_UART_DMA_CLK_ENABLE    __HAL_RCC_DMA2_CLK_ENABLE
#define U_PORT_UART_RX_DMA_STREAM     DMA2_Stream2
#define U_PORT_UART_RX_DMA_CHANNEL    DMA_CHANNEL_5
#define U_PORT_UART_RX_DMA_IRQn       DMA2_Stream2_IRQn
#define U_PORT_UART_RX_DMA_IRQHandler DMA2_Stream2_IRQHandler

void uPortUart_IRQHandler(void);
void uPortUartDma_IRQHandler(void);

void *pvPortMallocAligned32(size_t size);
void vPortFreeAligned32(void *aligned_ptr);

#ifdef __cplusplus
}
#endif

#endif // U_PORT_UART_STM32F779_H
