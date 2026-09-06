#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "u_port.h"

#define CHECK(condition)                                                        \
    do {                                                                        \
        if (!(condition)) {                                                     \
            fprintf(stderr, "%s:%d: check failed: %s\n",                     \
                    __FILE__, __LINE__, #condition);                            \
            return false;                                                       \
        }                                                                       \
    } while (0)

static uint64_t gTimeMs;
static uint32_t gAdvanceMs;
static uint32_t gClockCalls;

int __wrap_clock_gettime(clockid_t clockId, struct timespec *pTime)
{
    if (clockId != CLOCK_MONOTONIC_RAW) {
        return -1;
    }

    pTime->tv_sec = (time_t)(gTimeMs / 1000U);
    pTime->tv_nsec = (long)((gTimeMs % 1000U) * 1000000U);
    gTimeMs += gAdvanceMs;
    gClockCalls++;
    return 0;
}

static bool testBootAtZero(void)
{
    gTimeMs = 0;
    uPortInit();
    gTimeMs = 10;
    uPortInit();
    CHECK(uPortGetTickTimeMs() == 10);
    uPortDeinit();
    return true;
}

static bool testDayRollover(void)
{
    gTimeMs = 86399990;
    uPortInit();
    gTimeMs = 86400005;
    CHECK(uPortGetTickTimeMs() == 15);
    uPortDeinit();
    return true;
}

static bool testTimerWrap(void)
{
    gTimeMs = UINT32_MAX - 5U;
    uPortInit();
    gTimeMs += 10U;
    CHECK(uPortGetTickTimeMs() == 10);
    return true;
}

static bool testSleep(void)
{
    gClockCalls = 0;
    CHECK(uPortSleepMs(0) == 0);
    CHECK(uPortSleepMs(-1) == 0);
    CHECK(gClockCalls == 0);

    gTimeMs = UINT32_MAX - 2U;
    gAdvanceMs = 1;
    CHECK(uPortSleepMs(5) == 0);
    CHECK(gClockCalls == 6);
    return true;
}

static bool testMutexAndRxStubs(void)
{
    bool mutex;
    U_CX_MUTEX_CREATE(mutex);
    CHECK(!mutex);
    CHECK(U_CX_MUTEX_TRY_LOCK(mutex, 0) == 0);
    CHECK(mutex);
    CHECK(U_CX_MUTEX_TRY_LOCK(mutex, UINT32_MAX) < 0);
    CHECK(mutex);
    U_CX_MUTEX_UNLOCK(mutex);
    CHECK(!mutex);
    U_CX_MUTEX_LOCK(mutex);
    CHECK(mutex);
    U_CX_MUTEX_UNLOCK(mutex);
    U_CX_MUTEX_DELETE(mutex);

    uPortBgRxTaskCreate(NULL);
    uPortBgRxTaskDestroy(NULL);
    return true;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        return EXIT_FAILURE;
    }

    bool passed = false;
    if (strcmp(argv[1], "boot-at-zero") == 0) {
        passed = testBootAtZero();
    } else if (strcmp(argv[1], "day-rollover") == 0) {
        passed = testDayRollover();
    } else if (strcmp(argv[1], "timer-wrap") == 0) {
        passed = testTimerWrap();
    } else if (strcmp(argv[1], "sleep") == 0) {
        passed = testSleep();
    } else if (strcmp(argv[1], "mutex-and-rx") == 0) {
        passed = testMutexAndRxStubs();
    }

    return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
