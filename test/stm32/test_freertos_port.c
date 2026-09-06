#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fake_freertos.h"
#include "semphr.h"
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

static atomic_int gRxCalls;

static void hostSleepMs(int32_t milliseconds)
{
    struct timespec delay = {
        .tv_sec = milliseconds / 1000,
        .tv_nsec = (milliseconds % 1000) * 1000000
    };
    nanosleep(&delay, NULL);
}

int32_t uCxAtClientHandleRx(uCxAtClient_t *pClient)
{
    (void)pClient;
    atomic_fetch_add(&gRxCalls, 1);
    return 0;
}

static bool testTime(void)
{
    fakeRtosReset();
    fakeRtosSetTickCount(100);
    uPortInit();
    CHECK(uPortGetTickTimeMs() == 0);
    fakeRtosSetTickCount(137);
    CHECK(uPortGetTickTimeMs() == 37);
    uPortInit();
    CHECK(uPortGetTickTimeMs() == 37);
    uPortDeinit();
    return true;
}

static bool testMutexTimeouts(void)
{
    SemaphoreHandle_t semaphore = (SemaphoreHandle_t)(uintptr_t)1;

    fakeRtosSetSemaphoreResult(pdTRUE);
    CHECK(uPortMutexTryLock(semaphore, 0) == 0);
    CHECK(fakeRtosGetLastSemaphoreTimeout() == 0);
    CHECK(uPortMutexTryLock(semaphore, UINT32_MAX) == 0);
    CHECK(fakeRtosGetLastSemaphoreTimeout() == portMAX_DELAY);
    CHECK(uPortMutexTryLock(semaphore, 25) == 0);
    CHECK(fakeRtosGetLastSemaphoreTimeout() == pdMS_TO_TICKS(25));

    fakeRtosSetSemaphoreResult(pdFALSE);
    CHECK(uPortMutexTryLock(semaphore, 10) < 0);
    return true;
}

static bool testBackgroundRxTask(void)
{
    uCxAtClient_t client = {0};
    atomic_store(&gRxCalls, 0);
    fakeRtosReset();

    uPortBgRxTaskCreate(&client);
    uPortBgRxTaskCreate(&client);
    CHECK(fakeRtosGetTaskCreateCalls() == 1);
    CHECK(strcmp(fakeRtosGetTaskName(), "ucxRx") == 0);
    CHECK(fakeRtosGetTaskStackDepth() == 2048);
    CHECK(fakeRtosGetTaskPriority() == configMAX_PRIORITIES - 2U);

    for (int32_t attempt = 0;
         (attempt < 100) && (atomic_load(&gRxCalls) == 0);
         attempt++) {
        hostSleepMs(1);
    }
    CHECK(atomic_load(&gRxCalls) > 0);
    CHECK(fakeRtosGetDelayCalls() > 0);
    CHECK(fakeRtosGetLastDelay() == pdMS_TO_TICKS(10));

    uPortBgRxTaskDestroy(&client);
    fakeRtosJoinTask();
    int callsAfterDestroy = atomic_load(&gRxCalls);
    hostSleepMs(3);
    CHECK(atomic_load(&gRxCalls) == callsAfterDestroy);
    return true;
}

static bool testRepeatedBackgroundRxLifecycle(void)
{
    uCxAtClient_t client = {0};

    for (int32_t iteration = 0; iteration < 100; iteration++) {
        atomic_store(&gRxCalls, 0);
        fakeRtosReset();
        uPortBgRxTaskCreate(&client);
        for (int32_t attempt = 0;
             (attempt < 100) && (atomic_load(&gRxCalls) == 0);
             attempt++) {
            hostSleepMs(1);
        }
        CHECK(atomic_load(&gRxCalls) > 0);
        uPortBgRxTaskDestroy(&client);
        fakeRtosJoinTask();
    }
    return true;
}

static bool testBackgroundRxCreateFailure(void)
{
    uCxAtClient_t client = {0};
    atomic_store(&gRxCalls, 0);
    fakeRtosReset();
    fakeRtosSetTaskCreateResult(pdFAIL);

    uPortBgRxTaskCreate(&client);
    uPortBgRxTaskDestroy(&client);
    CHECK(atomic_load(&gRxCalls) == 0);
    CHECK(!fakeRtosGetStateCalledWithNull());
    return true;
}

int main(void)
{
    static const struct {
        const char *pName;
        bool (*pTest)(void);
    } tests[] = {
        {"time", testTime},
        {"mutex timeouts", testMutexTimeouts},
        {"background RX task", testBackgroundRxTask},
        {"repeated background RX lifecycle", testRepeatedBackgroundRxLifecycle},
        {"background RX create failure", testBackgroundRxCreateFailure}
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
