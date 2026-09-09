#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "fake_windows.h"
#include "u_cx_at_client.h"
#include "u_port.h"

#define CHECK(condition)                                                        \
    do {                                                                        \
        if (!(condition)) {                                                     \
            fprintf(stderr, "%s:%d: check failed: %s\n",                     \
                    __FILE__, __LINE__, #condition);                            \
            return false;                                                       \
        }                                                                       \
    } while (0)

int32_t uCxAtClientHandleRxAvailable(uCxAtClient_t *pClient)
{
    (void)pClient;
    gFakeWindows.rxCalls++;
    return 0;
}

int32_t uPortUartWaitForData(uPortUartHandle_t handle, int32_t timeoutMs)
{
    (void)handle;
    (void)timeoutMs;
    gFakeWindows.waitCommCalls++;
    return gFakeWindows.waitCommCalls == 1 ? 1 : -1;
}

void uPortUartWake(uPortUartHandle_t handle)
{
    (void)handle;
    gFakeWindows.cancelCalls++;
}

static bool testTimeAndMutex(void)
{
    fakeWindowsReset();
    gFakeWindows.performanceCounter = 100;
    uPortInit();
    CHECK(uPortGetTickTimeMs() == 0);
    gFakeWindows.performanceCounter = 137;
    CHECK(uPortGetTickTimeMs() == 37);

    HANDLE semaphore = CreateSemaphore(NULL, 1, 1, NULL);
    gFakeWindows.waitResult = WAIT_OBJECT_0;
    CHECK(uPortMutexTryLock(semaphore, -1) == 0);
    CHECK(gFakeWindows.lastWaitTimeout == INFINITE);
    gFakeWindows.waitResult = WAIT_TIMEOUT;
    CHECK(uPortMutexTryLock(semaphore, 25) == -2);
    CHECK(gFakeWindows.lastWaitTimeout == 25);
    gFakeWindows.waitResult = WAIT_FAILED;
    CHECK(uPortMutexTryLock(semaphore, 0) == -1);
    uPortDeinit();
    return true;
}

static bool testBackgroundRxLifecycle(void)
{
    uCxAtClient_t client = {0};
    client.uartHandle = (uPortUartHandle_t)(uintptr_t)1;

    fakeWindowsReset();
    uPortBgRxTaskCreate(&client);
    fakeWindowsRunThread();
    CHECK(gFakeWindows.rxCalls == 1);
    CHECK(gFakeWindows.waitCommCalls == 2);
    uPortBgRxTaskDestroy(&client);
    CHECK(gFakeWindows.cancelCalls == 1);
    CHECK(gFakeWindows.closeCalls == 1);
    CHECK(gFakeWindows.lastWaitTimeout == 5000);

    fakeWindowsReset();
    gFakeWindows.createThreadResult = FALSE;
    uPortBgRxTaskCreate(&client);
    uPortBgRxTaskDestroy(&client);
    CHECK(gFakeWindows.closeCalls == 0);
    return true;
}

int main(void)
{
    static const struct {
        const char *pName;
        bool (*pTest)(void);
    } tests[] = {
        {"time and mutex", testTimeAndMutex},
        {"background RX lifecycle", testBackgroundRxLifecycle}
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