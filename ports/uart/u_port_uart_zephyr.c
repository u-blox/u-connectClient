/*
 * Copyright 2025 u-blox
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @file
 * @brief Zephyr UART port implementation using Zephyr UART driver.
 *
 * This implementation uses static allocation and currently supports a single UART instance.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/ring_buffer.h>

#include "u_port_uart.h"

/* ----------------------------------------------------------------
 * COMPILE-TIME MACROS
 * -------------------------------------------------------------- */

#ifndef U_PORT_UART_RX_BUFFER_SIZE
/** RX ring size; 4 KB holds ~4 ms at 921600 baud plus the AT client's
 *  scheduling latency. Override with -DU_PORT_UART_RX_BUFFER_SIZE=... */
# define U_PORT_UART_RX_BUFFER_SIZE   4096
#endif

#ifndef U_PORT_UART_TX_BUFFER_SIZE
# define U_PORT_UART_TX_BUFFER_SIZE   1024
#endif

#ifndef U_PORT_UART_TX_TIMEOUT_MS
/** Max time uPortUartWrite() waits for TX ring space (e.g. CTS held off). */
# define U_PORT_UART_TX_TIMEOUT_MS    5000
#endif

/* ----------------------------------------------------------------
 * TYPES
 * -------------------------------------------------------------- */

/** Structure representing a UART handle.
 */
typedef struct {
    const struct device *pUartDev;
    struct ring_buf rxRingBuf;
    struct ring_buf txRingBuf;
    struct k_sem rxSem;
    struct k_sem txSem;
    struct k_mutex txMutex;
    uint8_t rxBuffer[U_PORT_UART_RX_BUFFER_SIZE];
    uint8_t txBuffer[U_PORT_UART_TX_BUFFER_SIZE];
} uPortUartHandle;

/* ----------------------------------------------------------------
 * STATIC VARIABLES
 * -------------------------------------------------------------- */

static uPortUartHandle gUartHandle = {0};

/* ----------------------------------------------------------------
 * STATIC FUNCTION PROTOTYPES
 * -------------------------------------------------------------- */

static void uartIsr(const struct device *dev, void *user_data);

/* ----------------------------------------------------------------
 * FORWARD DECLARATIONS
 * -------------------------------------------------------------- */

/* ----------------------------------------------------------------
 * STATIC FUNCTIONS
 * -------------------------------------------------------------- */

static void uartIsr(const struct device *dev, void *user_data)
{
    uPortUartHandle *pHandle = (uPortUartHandle *)user_data;
    int ret = 1;
    bool signalRxWorker = false;

    uart_irq_update(dev);
    while (uart_irq_rx_ready(dev) && (ret > 0)) {
        uint8_t *pData;
        signalRxWorker = true;
        ret = (int)ring_buf_put_ptr(&pHandle->rxRingBuf, &pData, 0);
        if (ret == 0) {
            // Ring buffer full
            uart_irq_rx_disable(dev);
        } else {
            ret = uart_fifo_read(dev, pData, ret);
        }
        ring_buf_commit(&pHandle->rxRingBuf, (ret > 0) ? (size_t)ret : 0);
        uart_irq_update(dev);
    }

    if (uart_irq_tx_ready(dev)) {
        uint8_t *pData;
        uint32_t len = ring_buf_get_ptr(&pHandle->txRingBuf, &pData, 0);
        if (len == 0) {
            uart_irq_tx_disable(dev);
        } else {
            int sent = uart_fifo_fill(dev, pData, (int)len);
            ring_buf_consume(&pHandle->txRingBuf, (sent > 0) ? (size_t)sent : 0);
        }
        k_sem_give(&pHandle->txSem);
    }

    if (signalRxWorker) {
        k_sem_give(&pHandle->rxSem);
        uPortUartRxSignalFromIsr();
    }
}

/* ----------------------------------------------------------------
 * PUBLIC FUNCTIONS
 * -------------------------------------------------------------- */

uPortUartHandle_t uPortUartOpen(const char *pDevName, int32_t baudRate, bool useFlowControl)
{
    if (pDevName == NULL) {
        return NULL;
    }

    // Use static handle - only supports one UART instance
    uPortUartHandle *pHandle = &gUartHandle;

    // Check if already in use
    if (pHandle->pUartDev != NULL) {
        return NULL;
    }

    memset(pHandle, 0, sizeof(uPortUartHandle));

    // Get UART device
    pHandle->pUartDev = device_get_binding(pDevName);
    if (pHandle->pUartDev == NULL) {
        return NULL;
    }

    if (!device_is_ready(pHandle->pUartDev)) {
        pHandle->pUartDev = NULL;
        return NULL;
    }

    // Configure UART
    struct uart_config config = {
        .baudrate = baudRate,
        .parity = UART_CFG_PARITY_NONE,
        .stop_bits = UART_CFG_STOP_BITS_1,
        .data_bits = UART_CFG_DATA_BITS_8,
        .flow_ctrl = useFlowControl ? UART_CFG_FLOW_CTRL_RTS_CTS : UART_CFG_FLOW_CTRL_NONE
    };

    if (uart_configure(pHandle->pUartDev, &config) < 0) {
        pHandle->pUartDev = NULL;
        return NULL;
    }

    // Initialize ring buffer and semaphore
    ring_buf_init(&pHandle->rxRingBuf, sizeof(pHandle->rxBuffer), pHandle->rxBuffer);
    ring_buf_init(&pHandle->txRingBuf, sizeof(pHandle->txBuffer), pHandle->txBuffer);
    k_sem_init(&pHandle->rxSem, 0, 1);
    k_sem_init(&pHandle->txSem, 0, 1);
    k_mutex_init(&pHandle->txMutex);

    // Set up interrupt callback
    if (uart_irq_callback_user_data_set(pHandle->pUartDev, uartIsr, pHandle) < 0) {
        pHandle->pUartDev = NULL;
        return NULL;
    }

    // Enable RX interrupts
    uart_irq_rx_enable(pHandle->pUartDev);

    return (uPortUartHandle_t)pHandle;
}

void uPortUartClose(uPortUartHandle_t handle)
{
    if (handle != NULL) {
        uPortUartHandle *pHandle = (uPortUartHandle *)handle;
        // Let queued TX drain before shutting the interrupts off
        int64_t endMs = k_uptime_get() + 100;
        while (!ring_buf_is_empty(&pHandle->txRingBuf) && (k_uptime_get() < endMs)) {
            k_sleep(K_MSEC(1));
        }
        uart_irq_tx_disable(pHandle->pUartDev);
        uart_irq_rx_disable(pHandle->pUartDev);
        pHandle->pUartDev = NULL;
    }
}

int32_t uPortUartWrite(uPortUartHandle_t handle,
                       const void *pData,
                       size_t length)
{
    if ((handle == NULL) || (pData == NULL) || (length == 0)) {
        return -1;
    }

    // uart_fifo_fill() may only be called from the ISR and accepts as many
    // bytes as the hardware FIFO has room for (often 1 on STM32), so queue
    // everything in the TX ring and let the ISR drain it. Returns the full
    // length unless the transmitter stalls for U_PORT_UART_TX_TIMEOUT_MS.
    uPortUartHandle *pHandle = (uPortUartHandle *)handle;
    const uint8_t *pBytes = (const uint8_t *)pData;
    size_t written = 0;
    int64_t lastProgressMs = k_uptime_get();

    k_mutex_lock(&pHandle->txMutex, K_FOREVER);
    while (written < length) {
        uint32_t put = ring_buf_put(&pHandle->txRingBuf, &pBytes[written],
                                    (uint32_t)(length - written));
        if (put > 0) {
            written += put;
            lastProgressMs = k_uptime_get();
            uart_irq_tx_enable(pHandle->pUartDev);
        } else {
            if ((k_uptime_get() - lastProgressMs) > U_PORT_UART_TX_TIMEOUT_MS) {
                break;
            }
            uart_irq_tx_enable(pHandle->pUartDev);
            k_sem_take(&pHandle->txSem, K_MSEC(10));
        }
    }
    k_mutex_unlock(&pHandle->txMutex);

    return (written > 0) ? (int32_t)written : -1;
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
    int32_t bytesRead = 0;

    if (timeoutMs > 0) {
        uint8_t *pDataBytes = (uint8_t *)pData;
        uint32_t startTimeMs = k_uptime_get_32();
        int32_t timePassedMs = 0;

        while (bytesRead < (int32_t)length) {
            if (ring_buf_is_empty(&pHandle->rxRingBuf)) {
                if (timePassedMs > timeoutMs) {
                    // Timeout
                    break;
                }
                int32_t remainingMs = timeoutMs - timePassedMs;
                k_sem_take(&pHandle->rxSem, K_MSEC(remainingMs));
            } else {
                if (pDataBytes != NULL) {
                    uart_irq_rx_disable(pHandle->pUartDev);
                    bytesRead += (int32_t)ring_buf_get(&pHandle->rxRingBuf, 
                                                        &pDataBytes[bytesRead], 
                                                        length - (size_t)bytesRead);
                    uart_irq_rx_enable(pHandle->pUartDev);
                } else {
                    // NULL buffer - just return 0
                    break;
                }
            }
            timePassedMs = (int32_t)(k_uptime_get_32() - startTimeMs);
        }
    } else {
        if (!ring_buf_is_empty(&pHandle->rxRingBuf) && (pData != NULL)) {
            uart_irq_rx_disable(pHandle->pUartDev);
            bytesRead = (int32_t)ring_buf_get(&pHandle->rxRingBuf, pData, length);
            uart_irq_rx_enable(pHandle->pUartDev);
        }
    }

    return bytesRead;
}

int32_t uPortUartWaitForData(uPortUartHandle_t handle, int32_t timeoutMs)
{
    if (handle == NULL) {
        return -1;
    }

    uPortUartHandle *pHandle = (uPortUartHandle *)handle;
    if (!ring_buf_is_empty(&pHandle->rxRingBuf)) {
        return 1;
    }

    k_timeout_t timeout = timeoutMs < 0 ? K_FOREVER : K_MSEC(timeoutMs);
    return k_sem_take(&pHandle->rxSem, timeout) == 0 ? 1 : 0;
}

void uPortUartWake(uPortUartHandle_t handle)
{
    if (handle != NULL) {
        uPortUartHandle *pHandle = (uPortUartHandle *)handle;
        k_sem_give(&pHandle->rxSem);
    }
}
