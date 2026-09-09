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
 * @brief STM32F779 UART port implementation using HAL.
 *
 * USART6 with circular RX DMA (DMA2 Stream2 / Channel 5) into a
 * 32-byte-aligned ring buffer. D-cache coherency is handled with
 * SCB_InvalidateDCache_by_Addr() before every CPU read of the buffer.
 * FreeRTOS-based (heap allocation via pvPortMalloc, delays via vTaskDelay).
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f7xx_hal.h"
#include "u_port_uart.h"
#include "u_port_uart_stm32f779.h"

#include "u_port.h"
#include "u_cx_log.h"

/* ----------------------------------------------------------------
 * COMPILE-TIME MACROS
 * -------------------------------------------------------------- */

/* ----------------------------------------------------------------
 * TYPES
 * -------------------------------------------------------------- */

/* ----------------------------------------------------------------
 * STATIC VARIABLES
 * -------------------------------------------------------------- */

static uPortUartHandle *gpUartHandle = NULL;

/* ----------------------------------------------------------------
 * STATIC FUNCTION PROTOTYPES
 * -------------------------------------------------------------- */

static uint32_t getDmaWriteCount(uPortUartHandle *pHandle);
static uint32_t getRxBufferAvailable(uPortUartHandle *pHandle);
static void startRxDma(uPortUartHandle *pHandle);

/* ----------------------------------------------------------------
 * STATIC FUNCTIONS
 * -------------------------------------------------------------- */

/** Total bytes written to the ring buffer by DMA (mod 2^32).
 *  Reads wrap counter and DMA NDTR consistently (retries if a
 *  buffer wrap happens between the two reads).
 */
static uint32_t getDmaWriteCount(uPortUartHandle *pHandle)
{
    uint32_t wraps;
    uint32_t ndtr;

    do {
        wraps = pHandle->rxWraps;
        ndtr = __HAL_DMA_GET_COUNTER(pHandle->huart.hdmarx);
    } while (wraps != pHandle->rxWraps);

    return (wraps * U_PORT_UART_RX_BUFFER_SIZE) +
           (U_PORT_UART_RX_BUFFER_SIZE - ndtr);
}

static uint32_t getRxBufferAvailable(uPortUartHandle *pHandle)
{
    if (pHandle->rxResync) {
        // UART error occurred and DMA reception was restarted:
        // discard everything received before the error.
        pHandle->rxResync = false;
        pHandle->rxTotalRead = getDmaWriteCount(pHandle);

        // Deferred from HAL_UART_ErrorCallback (ISR context) - safe to printf here.
        printf("[UART] RX error #%lu ErrorCode=0x%02lX (ORE=%d FE=%d NE=%d PE=%d) - DMA restarted, resyncing\r\n",
               (unsigned long)pHandle->errorCount, (unsigned long)pHandle->lastErrorCode,
               (pHandle->lastErrorCode & HAL_UART_ERROR_ORE) != 0, (pHandle->lastErrorCode & HAL_UART_ERROR_FE) != 0,
               (pHandle->lastErrorCode & HAL_UART_ERROR_NE) != 0, (pHandle->lastErrorCode & HAL_UART_ERROR_PE) != 0);

        return 0;
    }

    uint32_t writeCount = getDmaWriteCount(pHandle); // total bytes DMA has ever written
    uint32_t available = writeCount - pHandle->rxTotalRead; // new unread bytes

    if (available > U_PORT_UART_RX_BUFFER_SIZE) {
        // Hardware reloads NDTR to full the instant a circular wrap
        // completes, but rxWraps is only incremented later inside the DMA
        // transfer-complete ISR. Sampling in that gap makes the computed
        // write count undershoot by one full buffer, which looks like a
        // huge unsigned "overflow" here but is not a real one. A genuine
        // reader-too-slow overflow persists; this race self-heals within
        // microseconds once the pending ISR runs, so retry first.
        for (int retry = 0; retry < 100 && available > U_PORT_UART_RX_BUFFER_SIZE; retry++) {
            available = getDmaWriteCount(pHandle) - pHandle->rxTotalRead;
        }
    }
    if (available > U_PORT_UART_RX_BUFFER_SIZE) {
        // Still bad after retries - DMA has genuinely lapped the reader and
        // buffer content is no longer coherent. Drop it all rather than
        // deliver corrupt data.
        pHandle->overflowCount++;
        // Diagnostic only - this is a SILENT discard path distinct from
        // HAL_UART_ErrorCallback; confirms/refutes reader-too-slow as root cause.
        printf("[UART] RX OVERFLOW #%lu: reader lapped by DMA (available=%lu > bufsize=%u) - discarding\r\n",
               (unsigned long)pHandle->overflowCount, (unsigned long)available,
               (unsigned)U_PORT_UART_RX_BUFFER_SIZE);
        pHandle->rxTotalRead = getDmaWriteCount(pHandle);
        return 0;
    }
    return available;
}

static void startRxDma(uPortUartHandle *pHandle)
{
    pHandle->rxWraps = 0;
    HAL_UART_Receive_DMA(&pHandle->huart, pHandle->rxBuffer,
                         U_PORT_UART_RX_BUFFER_SIZE);
}

/* ----------------------------------------------------------------
 * PUBLIC FUNCTIONS
 * -------------------------------------------------------------- */

uPortUartHandle_t uPortUartOpen(const char *pDevice, int32_t baudRate, bool useFlowControl)
{
    (void)pDevice;  // Device name not used on embedded systems

    if (gpUartHandle != NULL) {
        // Only one UART instance supported
        printf("UART already opened\n");
        return NULL;
    }

    uPortUartHandle *pHandle = (uPortUartHandle *)pvPortMallocAligned32(sizeof(uPortUartHandle));
    if (pHandle == NULL) {
        printf("Failed to allocate memory for UART handle\n");
        return NULL; 
    }

    memset(pHandle, 0, sizeof(uPortUartHandle));

    __HAL_RCC_USART6_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    /**USART6 GPIO Configuration  
    PG9    ------> USART6_RX
    PG12   ------> USART6_RTS
    PG13   ------> USART6_CTS
    PG14   ------> USART6_TX 
    */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6 ;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    pHandle->huart.Instance = USART6;
    pHandle->huart.Init.BaudRate = baudRate;
    pHandle->huart.Init.WordLength = UART_WORDLENGTH_8B;
    pHandle->huart.Init.StopBits = UART_STOPBITS_1;
    pHandle->huart.Init.Parity = UART_PARITY_NONE;
    pHandle->huart.Init.Mode = UART_MODE_TX_RX;

    if (useFlowControl) 
    {
        pHandle->huart.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
    } 
    else 
    {
        pHandle->huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    }  

    pHandle->huart.Init.OverSampling = UART_OVERSAMPLING_16;
    pHandle->huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    pHandle->huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&pHandle->huart) != HAL_OK)
    {
        vPortFreeAligned32(pHandle);
        return NULL;
    }

    /* USART6 DMA Init */
    /* USART6_RX Init */
    U_PORT_UART_DMA_CLK_ENABLE();
    pHandle->hdmaRx.Instance = U_PORT_UART_RX_DMA_STREAM;
    pHandle->hdmaRx.Init.Channel = U_PORT_UART_RX_DMA_CHANNEL;
    pHandle->hdmaRx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    pHandle->hdmaRx.Init.PeriphInc = DMA_PINC_DISABLE;
    pHandle->hdmaRx.Init.MemInc = DMA_MINC_ENABLE;
    pHandle->hdmaRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    pHandle->hdmaRx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    pHandle->hdmaRx.Init.Mode = DMA_CIRCULAR;
    pHandle->hdmaRx.Init.Priority = DMA_PRIORITY_HIGH;
    pHandle->hdmaRx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&pHandle->hdmaRx) != HAL_OK) {
        HAL_UART_DeInit(&pHandle->huart);
        vPortFreeAligned32(pHandle);
        return NULL;
    }

    __HAL_LINKDMA(&pHandle->huart, hdmarx, pHandle->hdmaRx);

    // Enable UART + DMA interrupts
    // Priority must be >= configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY (5) for FreeRTOS compatibility
    // Using priority 6 to ensure it's lower priority than FreeRTOS syscalls
    HAL_NVIC_SetPriority(U_PORT_UART_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(U_PORT_UART_IRQn);
    HAL_NVIC_SetPriority(U_PORT_UART_RX_DMA_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(U_PORT_UART_RX_DMA_IRQn);

    pHandle->isOpen = true;
    gpUartHandle = pHandle;

    // Start receiving
    startRxDma(pHandle);

    return (uPortUartHandle_t)pHandle;
}

void uPortUartClose(uPortUartHandle_t handle)
{
    if (handle != NULL) {
        uPortUartHandle *pHandle = (uPortUartHandle *)handle;

        if (pHandle->isOpen) {
            HAL_UART_DMAStop(&pHandle->huart);
            HAL_NVIC_DisableIRQ(U_PORT_UART_RX_DMA_IRQn);
            HAL_NVIC_DisableIRQ(U_PORT_UART_IRQn);
            HAL_DMA_DeInit(&pHandle->hdmaRx);
            HAL_UART_DeInit(&pHandle->huart);
            U_PORT_UART_CLK_DISABLE();
            pHandle->isOpen = false;
        }

        if (gpUartHandle == pHandle) {
            gpUartHandle = NULL;
        }

        vPortFreeAligned32(pHandle);
    }
}

int32_t uPortUartWrite(uPortUartHandle_t handle,
                       const void *pData,
                       size_t length)
{
    if ((handle == NULL) || (pData == NULL) || (length == 0)) {
        return -1;
    }

    uPortUartHandle *pHandle = (uPortUartHandle *)handle;

    if (!pHandle->isOpen) {
        return -1;
    }

    HAL_StatusTypeDef status = HAL_UART_Transmit(&pHandle->huart, (uint8_t *)pData, (uint16_t)length, HAL_MAX_DELAY);

    if (status != HAL_OK) {
        return -1;
    }

    return (int32_t)length;
}

int32_t uPortUartRead(uPortUartHandle_t handle,
                      void *pData,
                      size_t length,
                      int32_t timeoutMs)
{
    if ((handle == NULL) || (length == 0)) {
        return -1;
    }

    uPortUartHandle *pHandle = (uPortUartHandle *)handle;

    if (!pHandle->isOpen) {
        return -1;
    }

    // Check available data
    uint32_t available = getRxBufferAvailable(pHandle);

    if (timeoutMs == 0) {
        // Non-blocking: return immediately
        if (available == 0) {
            return 0;
        }
    }

    // If pData is NULL, just return 0 (test case)
    if (pData == NULL) {
        return 0;
    }

    // Wait for data if blocking
    if (timeoutMs > 0 && available == 0) {
        uint32_t startTime = HAL_GetTick();
        while (available == 0) {
            vTaskDelay(pdMS_TO_TICKS(1));  // Sleep for 1 ms to avoid busy waiting
            available = getRxBufferAvailable(pHandle);
            if ((HAL_GetTick() - startTime) >= (uint32_t)timeoutMs) {
                return 0;  // Timeout
            }
        }
    }

    // Read data from circular buffer (may need two copies at wrap point)
    uint32_t bytesToRead = (length < available) ? length : available;
    uint32_t tailIdx = pHandle->rxTotalRead % U_PORT_UART_RX_BUFFER_SIZE;
    uint32_t firstChunk = U_PORT_UART_RX_BUFFER_SIZE - tailIdx;
    if (firstChunk > bytesToRead) {
        firstChunk = bytesToRead;
    }

    // 1. CRITICAL: Invalidate the specific buffer segment BEFORE the CPU reads it.
    SCB_InvalidateDCache_by_Addr((uint32_t*)&pHandle->rxBuffer[tailIdx], firstChunk);

    // 2. Safely read and process your data here...
    memcpy(pData, &pHandle->rxBuffer[tailIdx], firstChunk);
    if (bytesToRead > firstChunk) {
        SCB_InvalidateDCache_by_Addr((uint32_t*)&pHandle->rxBuffer[0], bytesToRead - firstChunk);
        memcpy((uint8_t *)pData + firstChunk, &pHandle->rxBuffer[0],
               bytesToRead - firstChunk);
    }

    pHandle->rxTotalRead += bytesToRead;

    return (int32_t)bytesToRead;
}

/* ----------------------------------------------------------------
 * UART INTERRUPT CALLBACKS
 * -------------------------------------------------------------- */

/**
 * @brief DMA transfer complete callback (circular mode = buffer wrap)
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (gpUartHandle != NULL && huart->Instance == gpUartHandle->huart.Instance) {
        gpUartHandle->rxWraps++;
    }
}

/**
 * @brief UART error callback (overrun, framing, noise, DMA error)
 *
 * Without this callback a single overrun error would abort DMA reception
 * permanently and the UART would go silently deaf. Restart reception and
 * let the reader resynchronize.
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (gpUartHandle != NULL && huart->Instance == gpUartHandle->huart.Instance) {
        gpUartHandle->errorCount++;
        gpUartHandle->lastErrorCode = huart->ErrorCode;
        gpUartHandle->rxResync = true;
        // No printf() here: this runs in ISR context and blocking the IRQ on a
        // (possibly semihosted/UART-blocking/lock-taking) console write only
        // delays the DMA restart below, risking losing more bytes on top of
        // the error that just happened. The latched code/count are printed
        // once from task context in getRxBufferAvailable()'s resync branch.
        // HAL has already aborted the transfer at this point; clear any
        // remaining error flags and restart circular DMA reception.
        __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF | UART_CLEAR_NEF | UART_CLEAR_FEF);
        HAL_UART_DMAStop(huart);
        startRxDma(gpUartHandle);
    }
}

/* ----------------------------------------------------------------
 * UART INTERRUPT HANDLERS
 * -------------------------------------------------------------- */

/**
 * @brief UART interrupt handler
 *
 * This function must be called from your UART IRQ handler in your
 * main application code (e.g., in stm32h7xx_it.c):
 *
 * void USART3_IRQHandler(void)
 * {
 *     uPortUart_IRQHandler();
 * }
 */
void uPortUart_IRQHandler(void)
{
    if (gpUartHandle != NULL) {
        HAL_UART_IRQHandler(&gpUartHandle->huart);
    }
}

/**
 * @brief RX DMA stream interrupt handler
 *
 * This function must be called from the RX DMA stream IRQ handler in your
 * main application code (e.g., in stm32h7xx_it.c):
 *
 * void DMA1_Stream0_IRQHandler(void)
 * {
 *     uPortUartDma_IRQHandler();
 * }
 */
void uPortUartDma_IRQHandler(void)
{
    if (gpUartHandle != NULL) {
        HAL_DMA_IRQHandler(&gpUartHandle->hdmaRx);
    }
}

/* ----------------------------------------------------------------
 * UART FLUSH FUNCTIONS
 * -------------------------------------------------------------- */

void uPortUartFlushRx(uPortUartHandle_t handle)
{
    if (handle == NULL) {
        return;
    }

    uPortUartHandle *pHandle = (uPortUartHandle *)handle;

    if (!pHandle->isOpen) {
        return;
    }

    // Discard anything currently sitting in the DMA ring buffer by
    // fast-forwarding the read position to the current DMA write count.
    pHandle->rxTotalRead = getDmaWriteCount(pHandle);
}

void *pvPortMallocAligned32(size_t size) {
    // Allocate extra space for alignment adjustments and storing the original pointer
    void *original_ptr = pvPortMalloc(size + 32 + sizeof(void*));
    if (original_ptr == NULL) {
        return NULL;
    }

    // Calculate a 32-byte aligned address after reserving room for the original pointer
    void *aligned_ptr = (void *)(((uintptr_t)original_ptr + sizeof(void*) + 31) & ~31);

    // Save the original pointer right before the aligned memory block so we can free it later
    ((void **)aligned_ptr)[-1] = original_ptr;

    return aligned_ptr;
}

void vPortFreeAligned32(void *aligned_ptr) {
    if (aligned_ptr != NULL) {
        // Retrieve and free the original pointer allocated by FreeRTOS
        void *original_ptr = ((void **)aligned_ptr)[-1];
        vPortFree(original_ptr);
    }
}