#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "stm32f4xx_hal.h"
#include "u_port_uart.h"

#define RX_BUFFER_CAPACITY 2047U
#define CHECK(condition)                                                        \
    do {                                                                        \
        if (!(condition)) {                                                     \
            fprintf(stderr, "%s:%d: check failed: %s\n",                     \
                    __FILE__, __LINE__, #condition);                            \
            return false;                                                       \
        }                                                                       \
    } while (0)

extern void HAL_UART_RxCpltCallback(UART_HandleTypeDef *pUart);
extern void uPortUart_IRQHandler(void);

typedef struct {
    uPortUartHandle_t handle;
    uint8_t value;
    int32_t result;
} blockingRead_t;

static void sleepMs(int32_t milliseconds)
{
    struct timespec delay = {
        .tv_sec = milliseconds / 1000,
        .tv_nsec = (milliseconds % 1000) * 1000000
    };
    nanosleep(&delay, NULL);
}

static bool testOpenConfiguration(void)
{
    fakeHalReset();
    uPortUartHandle_t handle = uPortUartOpen(NULL, 921600, false);
    CHECK(handle != NULL);
    CHECK(gFakeHal.initCalls == 1);
    CHECK(gFakeHal.clockEnableCalls == 1);
    CHECK(gFakeHal.pLastUart != NULL);
    CHECK(gFakeHal.pLastUart->Instance == USART3);
    CHECK(gFakeHal.pLastUart->Init.BaudRate == 921600U);
    CHECK(gFakeHal.pLastUart->Init.WordLength == UART_WORDLENGTH_8B);
    CHECK(gFakeHal.pLastUart->Init.StopBits == UART_STOPBITS_1);
    CHECK(gFakeHal.pLastUart->Init.Parity == UART_PARITY_NONE);
    CHECK(gFakeHal.pLastUart->Init.Mode == UART_MODE_TX_RX);
    CHECK(gFakeHal.pLastUart->Init.HwFlowCtl == UART_HWCONTROL_NONE);
    CHECK(gFakeHal.pLastUart->Init.OverSampling == UART_OVERSAMPLING_16);
    CHECK(gFakeHal.nvicSetPriorityCalls == 1);
    CHECK(gFakeHal.lastIrq == USART3_IRQn);
    CHECK(gFakeHal.lastPriority == 6);
    CHECK(gFakeHal.nvicEnableCalls == 1);
    CHECK(gFakeHal.receiveCalls == 1);
    CHECK(uPortUartOpen(NULL, 115200, false) == NULL);

    uPortUartClose(handle);
    CHECK(gFakeHal.nvicDisableCalls == 1);
    CHECK(gFakeHal.deinitCalls == 1);
    CHECK(gFakeHal.clockDisableCalls == 1);
    uPortUartClose(NULL);

    fakeHalReset();
    handle = uPortUartOpen(NULL, 115200, true);
    CHECK(handle != NULL);
    CHECK(gFakeHal.pLastUart->Init.HwFlowCtl == UART_HWCONTROL_RTS_CTS);
    uPortUartClose(handle);
    return true;
}

static bool testOpenFailures(void)
{
    fakeHalReset();
    gFakeHal.initStatus = HAL_ERROR;
    CHECK(uPortUartOpen(NULL, 115200, false) == NULL);
    CHECK(gFakeHal.clockEnableCalls == 1);
    CHECK(gFakeHal.clockDisableCalls == 1);

    fakeHalReset();
    gFakeHal.receiveStatus = HAL_ERROR;
    CHECK(uPortUartOpen(NULL, 115200, false) == NULL);
    CHECK(gFakeHal.deinitCalls == 1);
    CHECK(gFakeHal.clockDisableCalls == 1);

    fakeHalReset();
    uPortUartHandle_t handle = uPortUartOpen(NULL, 115200, false);
    CHECK(handle != NULL);
    uPortUartClose(handle);
    return true;
}

static bool testWrite(void)
{
    static uint8_t payload[] = {0x00, 0x0d, 0x0a, 0x80, 0xff};

    fakeHalReset();
    uPortUartHandle_t handle = uPortUartOpen(NULL, 115200, false);
    CHECK(handle != NULL);
    CHECK(uPortUartWrite(NULL, payload, sizeof(payload)) < 0);
    CHECK(uPortUartWrite(handle, NULL, sizeof(payload)) < 0);
    CHECK(uPortUartWrite(handle, payload, 0) < 0);

    CHECK(uPortUartWrite(handle, payload, sizeof(payload)) ==
          (int32_t)sizeof(payload));
    CHECK(gFakeHal.transmitCalls == 1);
    CHECK(gFakeHal.pTxData == payload);
    CHECK(gFakeHal.txLength == sizeof(payload));
    CHECK(gFakeHal.txTimeout == HAL_MAX_DELAY);

    gFakeHal.transmitStatus = HAL_TIMEOUT;
    CHECK(uPortUartWrite(handle, payload, sizeof(payload)) < 0);
    gFakeHal.transmitStatus = HAL_OK;

    uint8_t *pLargePayload = malloc((size_t)UINT16_MAX + 1U);
    CHECK(pLargePayload != NULL);
        uint32_t transmitCalls = gFakeHal.transmitCalls;
        CHECK(uPortUartWrite(handle, pLargePayload, (size_t)UINT16_MAX + 1U) ==
            (int32_t)((size_t)UINT16_MAX + 1U));
        CHECK(gFakeHal.transmitCalls == transmitCalls + 2U);
        CHECK(gFakeHal.txLength == 1U);
    free(pLargePayload);

    uPortUartClose(handle);
    return true;
}

static bool testFragmentedBinaryRead(void)
{
    static const uint8_t payload[] = {0x00, 0x0d, 0x0a, 0x7f, 0x80, 0xff};
    uint8_t received[sizeof(payload)] = {0};

    fakeHalReset();
    uPortUartHandle_t handle = uPortUartOpen(NULL, 115200, false);
    CHECK(handle != NULL);
    CHECK(uPortUartRead(NULL, received, sizeof(received), 0) < 0);
    CHECK(uPortUartRead(handle, received, 0, 0) < 0);
    CHECK(uPortUartRead(handle, received, sizeof(received), 0) == 0);
    CHECK(uPortUartRead(handle, NULL, sizeof(received), 0) == 0);

    for (size_t index = 0; index < 2; index++) {
        fakeHalInjectRxByte(payload[index]);
    }
    CHECK(uPortUartRead(handle, received, sizeof(received), 0) == 2);
    for (size_t index = 2; index < sizeof(payload); index++) {
        fakeHalInjectRxByte(payload[index]);
    }
    CHECK(uPortUartRead(handle, received + 2, sizeof(received) - 2, 0) == 4);
    CHECK(memcmp(received, payload, sizeof(payload)) == 0);
    CHECK(gFakeHal.receiveCalls == sizeof(payload) + 1U);

    uPortUartClose(handle);
    return true;
}

static bool testRingWrapAndOverflow(void)
{
    uint8_t received[RX_BUFFER_CAPACITY];

    fakeHalReset();
    uPortUartHandle_t handle = uPortUartOpen(NULL, 115200, false);
    CHECK(handle != NULL);

    for (uint32_t index = 0; index < RX_BUFFER_CAPACITY; index++) {
        fakeHalInjectRxByte((uint8_t)index);
    }
    fakeHalInjectRxByte(0xee);
    CHECK(uPortUartRead(handle, received, sizeof(received), 0) ==
          (int32_t)sizeof(received));
    for (uint32_t index = 0; index < RX_BUFFER_CAPACITY; index++) {
        CHECK(received[index] == (uint8_t)index);
    }
    CHECK(uPortUartRead(handle, received, sizeof(received), 0) == 0);

    for (uint32_t index = 0; index < 100; index++) {
        fakeHalInjectRxByte((uint8_t)(index + 1U));
    }
    CHECK(uPortUartRead(handle, received, 40, 0) == 40);
    CHECK(uPortUartRead(handle, received + 40, 60, 0) == 60);
    for (uint32_t index = 0; index < 100; index++) {
        CHECK(received[index] == (uint8_t)(index + 1U));
    }

    uPortUartClose(handle);
    return true;
}

static bool testTimeoutAndIrq(void)
{
    uint8_t received;

    fakeHalReset();
    uPortUartHandle_t handle = uPortUartOpen(NULL, 115200, false);
    CHECK(handle != NULL);
    gFakeHal.tick = UINT32_MAX - 2U;
    gFakeHal.tickIncrement = 1;
    CHECK(uPortUartRead(handle, &received, 1, 5) == 0);

    UART_HandleTypeDef otherUart = {.Instance = USART2};
    uint32_t receiveCalls = gFakeHal.receiveCalls;
    HAL_UART_RxCpltCallback(&otherUart);
    CHECK(gFakeHal.receiveCalls == receiveCalls);

    uPortUart_IRQHandler();
    CHECK(gFakeHal.irqHandlerCalls == 1);
    CHECK(gFakeHal.pLastUart->Instance == USART3);
    uPortUartClose(handle);
    uPortUart_IRQHandler();
    CHECK(gFakeHal.irqHandlerCalls == 1);
    return true;
}

static void *blockingRead(void *pParameter)
{
    blockingRead_t *pRead = pParameter;
    pRead->result = uPortUartRead(pRead->handle, &pRead->value, 1, -1);
    return NULL;
}

static bool testBlockingRead(void)
{
    fakeHalReset();
    blockingRead_t readParameters = {0};
    readParameters.handle = uPortUartOpen(NULL, 115200, false);
    CHECK(readParameters.handle != NULL);

    pthread_t reader;
    CHECK(pthread_create(&reader, NULL, blockingRead, &readParameters) == 0);
    sleepMs(10);
    fakeHalInjectRxByte(0xa5);
    CHECK(pthread_join(reader, NULL) == 0);
    CHECK(readParameters.result == 1);
    CHECK(readParameters.value == 0xa5);

    uPortUartClose(readParameters.handle);
    return true;
}

int main(void)
{
    static const struct {
        const char *pName;
        bool (*pTest)(void);
    } tests[] = {
        {"open configuration", testOpenConfiguration},
        {"open failures", testOpenFailures},
        {"write", testWrite},
        {"fragmented binary read", testFragmentedBinaryRead},
        {"ring wrap and overflow", testRingWrapAndOverflow},
        {"timeout and IRQ", testTimeoutAndIrq},
        {"blocking read", testBlockingRead}
    };

    for (size_t index = 0; index < sizeof(tests) / sizeof(tests[0]); index++) {
        printf("[ RUN      ] %s\n", tests[index].pName);
        if (!tests[index].pTest()) {
            printf("[  FAILED  ] %s\n", tests[index].pName);
            return EXIT_FAILURE;
        }
        printf("[       OK ] %s\n", tests[index].pName);
    }

    printf("[  PASSED  ] %zu tests\n", sizeof(tests) / sizeof(tests[0]));
    return EXIT_SUCCESS;
}
