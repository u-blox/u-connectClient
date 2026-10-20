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
 * @brief STM32F7 UART port configuration for NUCLEO-F767ZI
 *
 * Same wiring as the NUCLEO-H753ZI and NUCLEO-F439ZI ports so a single
 * NORA-W36 hookup (TX/RX/GND/3V3, no flow control) works on all three boards.
 *
 * UART Configuration:
 * -------------------
 * USART6 (u-blox NORA-W36 module communication):
 *   - PG14: TX (Arduino D1)
 *   - PG9:  RX (Arduino D0)
 *   - Baud: Configurable (2 Mbaud typical, no flow control)
 *
 * NOTE: On the NUCLEO-F767ZI the Arduino/Zio CN10 D0/D1 header pins map to
 * PG9/PG14 = USART6 (AF8) - NOT PB6/PB7 like the H7 board. Verified against
 * ST STM32F767ZITx.xml (ARDUINO_UNO_D0=PG_9, ARDUINO_UNO_D1=PG_14).
 *
 * USART3 (Console/Debug):
 *   - PD8: TX, PD9: RX (ST-LINK VCP)
 *   - Baud: 115200
 *   - Used for printf() output - see main_stm32.c
 */

#ifndef U_PORT_UART_STM32F7_H
#define U_PORT_UART_STM32F7_H

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------
 * COMPILE-TIME MACROS
 * -------------------------------------------------------------- */

/**
 * UART instance for u-blox module communication (USART6 PG9/PG14 = CN10 D0/D1).
 */
#define U_PORT_UART_INSTANCE    USART6
#define U_PORT_UART_IRQn        USART6_IRQn
#define U_PORT_UART_IRQHandler  USART6_IRQHandler
#define U_PORT_UART_CLK_ENABLE  __HAL_RCC_USART6_CLK_ENABLE
#define U_PORT_UART_CLK_DISABLE __HAL_RCC_USART6_CLK_DISABLE

/**
 * RX DMA configuration. Unlike the H7 (which routes any stream to any
 * peripheral via DMAMUX), STM32F7 uses fixed stream/channel mapping:
 * USART6_RX is DMA2 Stream 1, Channel 5 (RM0410 DMA2 request table).
 * RX uses circular DMA into a ring buffer so that no per-byte interrupts
 * are needed - required for reliable operation at high baud rates (2 Mbaud+).
 *
 * NOTE: the ring buffer must be DMA-accessible. On F7 the DMA controllers
 * cannot reach DTCM (0x20000000); the linker keeps .bss/heap in the main
 * SRAM1/SRAM2 region (0x20020000+) which DMA2 can reach.
 */
#define U_PORT_UART_DMA_CLK_ENABLE    __HAL_RCC_DMA2_CLK_ENABLE
#define U_PORT_UART_RX_DMA_STREAM     DMA2_Stream1
#define U_PORT_UART_RX_DMA_CHANNEL    DMA_CHANNEL_5
#define U_PORT_UART_RX_DMA_IRQn       DMA2_Stream1_IRQn
#define U_PORT_UART_RX_DMA_IRQHandler DMA2_Stream1_IRQHandler

#ifdef __cplusplus
}
#endif

#endif // U_PORT_UART_STM32F7_H
