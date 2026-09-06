#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <pty.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "u_cx_at_client.h"
#include "u_port.h"
#include "u_port_uart.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#define CHECK(condition)                                                        \
    do {                                                                        \
        if (!(condition)) {                                                     \
            fprintf(stderr, "%s:%d: check failed: %s\n",                     \
                    __FILE__, __LINE__, #condition);                            \
            return false;                                                       \
        }                                                                       \
    } while (0)

typedef struct {
    int fd;
    const uint8_t *pData;
    size_t length;
    int32_t delayMs;
} delayedWrite_t;

static atomic_int gRxCalls;
static atomic_int gRxFailures;
static atomic_bool gFailThreadCreate;
static atomic_int gThreadJoinCalls;

int __real_pthread_create(pthread_t *pThread, const pthread_attr_t *pAttributes,
                          void *(*pEntryPoint)(void *), void *pParameter);
int __real_pthread_join(pthread_t thread, void **pReturnValue);

int __wrap_pthread_create(pthread_t *pThread, const pthread_attr_t *pAttributes,
                          void *(*pEntryPoint)(void *), void *pParameter)
{
    if (atomic_exchange(&gFailThreadCreate, false)) {
        return EAGAIN;
    }
    return __real_pthread_create(pThread, pAttributes, pEntryPoint, pParameter);
}

int __wrap_pthread_join(pthread_t thread, void **pReturnValue)
{
    atomic_fetch_add(&gThreadJoinCalls, 1);
    return __real_pthread_join(thread, pReturnValue);
}

static int64_t monotonicTimeMs(void)
{
    struct timespec now;
    CHECK(clock_gettime(CLOCK_MONOTONIC, &now) == 0);
    return ((int64_t)now.tv_sec * 1000) + (now.tv_nsec / 1000000);
}

static bool sleepMs(int32_t delayMs)
{
    struct timespec delay = {
        .tv_sec = delayMs / 1000,
        .tv_nsec = (delayMs % 1000) * 1000000
    };

    while ((nanosleep(&delay, &delay) != 0) && (errno == EINTR)) {
    }
    return true;
}

static void *delayedWrite(void *pParameter)
{
    delayedWrite_t *pWrite = pParameter;
    size_t written = 0;

    sleepMs(pWrite->delayMs);
    while (written < pWrite->length) {
        ssize_t result = write(pWrite->fd, pWrite->pData + written,
                               pWrite->length - written);
        if (result > 0) {
            written += (size_t)result;
        } else if ((result < 0) && (errno != EINTR)) {
            break;
        }
    }
    return NULL;
}

static bool createPty(int *pMasterFd, char *pSlaveName, size_t nameLength)
{
    int slaveFd;

    CHECK(openpty(pMasterFd, &slaveFd, pSlaveName, NULL, NULL) == 0);
    CHECK(strlen(pSlaveName) < nameLength);
    CHECK(close(slaveFd) == 0);
    return true;
}

static bool testOpenAndConfiguration(void)
{
    static const int32_t baudRates[] = {
        9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600,
        1000000, 1500000, 2000000, 3000000
    };
    char slaveName[128];
    int masterFd;

    CHECK(uPortUartOpen(NULL, 115200, false) == NULL);
    CHECK(uPortUartOpen("/path/that/does/not/exist", 115200, false) == NULL);
    CHECK(createPty(&masterFd, slaveName, sizeof(slaveName)));
    CHECK(uPortUartOpen(slaveName, 12345, false) == NULL);

    for (size_t index = 0; index < ARRAY_SIZE(baudRates); index++) {
        uPortUartHandle_t handle = uPortUartOpen(slaveName, baudRates[index], false);
        CHECK(handle != NULL);
        uPortUartClose(handle);
    }

    uPortUartHandle_t handle = uPortUartOpen(slaveName, 115200, false);
    CHECK(handle != NULL);
    int observerFd = open(slaveName, O_RDWR | O_NOCTTY);
    CHECK(observerFd >= 0);
    struct termios settings;
    CHECK(tcgetattr(observerFd, &settings) == 0);
    CHECK((settings.c_lflag & (ICANON | ECHO | ECHOE | ISIG)) == 0);
    CHECK((settings.c_iflag &
           (IXON | IXOFF | IXANY | ICRNL | INLCR | IGNCR | ISTRIP)) == 0);
    CHECK((settings.c_oflag & OPOST) == 0);
    CHECK(settings.c_cc[VMIN] == 0);
    CHECK(settings.c_cc[VTIME] == 0);
    CHECK(close(observerFd) == 0);
    uPortUartClose(handle);

    handle = uPortUartOpen(slaveName, 115200, true);
    CHECK(handle != NULL);
    observerFd = open(slaveName, O_RDWR | O_NOCTTY);
    CHECK(observerFd >= 0);
    CHECK(tcgetattr(observerFd, &settings) == 0);
    CHECK((settings.c_cflag & CRTSCTS) != 0);
    CHECK(close(observerFd) == 0);
    uPortUartClose(handle);
    uPortUartClose(NULL);
    CHECK(close(masterFd) == 0);
    return true;
}

static bool testBinaryTransferAndReopen(void)
{
    static const uint8_t payload[] = {0x00, 0x0d, 0x0a, 0x7f, 0x80, 0xff};
    uint8_t received[sizeof(payload)] = {0};
    char slaveName[128];
    int masterFd;

    CHECK(createPty(&masterFd, slaveName, sizeof(slaveName)));
    uPortUartHandle_t handle = uPortUartOpen(slaveName, 115200, false);
    CHECK(handle != NULL);

    CHECK(write(masterFd, payload, 2) == 2);
    CHECK(uPortUartRead(handle, received, sizeof(received), 100) == 2);
    CHECK(memcmp(received, payload, 2) == 0);
    CHECK(write(masterFd, payload + 2, sizeof(payload) - 2) ==
          (ssize_t)(sizeof(payload) - 2));
    CHECK(uPortUartRead(handle, received + 2, sizeof(received) - 2, 100) ==
          (int32_t)(sizeof(received) - 2));
    CHECK(memcmp(received, payload, sizeof(payload)) == 0);

    CHECK(uPortUartWrite(handle, payload, sizeof(payload)) ==
          (int32_t)sizeof(payload));
    memset(received, 0, sizeof(received));
    CHECK(read(masterFd, received, sizeof(received)) == (ssize_t)sizeof(received));
    CHECK(memcmp(received, payload, sizeof(payload)) == 0);

    CHECK(uPortUartRead(handle, received, sizeof(received), 0) == 0);
    CHECK(uPortUartRead(handle, NULL, sizeof(received), 0) == 0);
    CHECK(uPortUartRead(NULL, received, sizeof(received), 0) < 0);
    CHECK(uPortUartRead(handle, received, 0, 0) < 0);
    CHECK(uPortUartWrite(NULL, payload, sizeof(payload)) < 0);
    CHECK(uPortUartWrite(handle, NULL, sizeof(payload)) < 0);
    CHECK(uPortUartWrite(handle, payload, 0) < 0);

    uPortUartClose(handle);
    handle = uPortUartOpen(slaveName, 115200, false);
    CHECK(handle != NULL);
    CHECK(write(masterFd, payload, sizeof(payload)) == (ssize_t)sizeof(payload));
    CHECK(uPortUartRead(handle, received, sizeof(received), 100) ==
          (int32_t)sizeof(received));
    CHECK(memcmp(received, payload, sizeof(payload)) == 0);

    uPortUartClose(handle);
    CHECK(close(masterFd) == 0);
    return true;
}

static bool testRepeatedOpenClose(void)
{
    uint8_t received;

    for (int32_t iteration = 0; iteration < 100; iteration++) {
        char slaveName[128];
        int masterFd;
        uint8_t expected = (uint8_t)iteration;

        CHECK(createPty(&masterFd, slaveName, sizeof(slaveName)));
        uPortUartHandle_t handle = uPortUartOpen(slaveName, 115200, false);
        CHECK(handle != NULL);
        CHECK(write(masterFd, &expected, 1) == 1);
        CHECK(uPortUartRead(handle, &received, 1, 100) == 1);
        CHECK(received == expected);
        uPortUartClose(handle);
        CHECK(close(masterFd) == 0);
    }
    return true;
}

static bool testReadTimeouts(void)
{
    static const uint8_t byte = 0xa5;
    uint8_t received = 0;
    char slaveName[128];
    int masterFd;

    CHECK(createPty(&masterFd, slaveName, sizeof(slaveName)));
    uPortUartHandle_t handle = uPortUartOpen(slaveName, 115200, false);
    CHECK(handle != NULL);

    int64_t startMs = monotonicTimeMs();
    CHECK(uPortUartRead(handle, &received, 1, 80) == 0);
    int64_t elapsedMs = monotonicTimeMs() - startMs;
    CHECK(elapsedMs >= 50);
    CHECK(elapsedMs < 500);

    delayedWrite_t writeParameters = {
        .fd = masterFd,
        .pData = &byte,
        .length = 1,
        .delayMs = 40
    };
    pthread_t writer;
    CHECK(pthread_create(&writer, NULL, delayedWrite, &writeParameters) == 0);
    startMs = monotonicTimeMs();
    CHECK(uPortUartRead(handle, &received, 1, 300) == 1);
    elapsedMs = monotonicTimeMs() - startMs;
    CHECK(pthread_join(writer, NULL) == 0);
    CHECK(received == byte);
    CHECK(elapsedMs >= 20);
    CHECK(elapsedMs < 250);

    uPortUartClose(handle);
    CHECK(close(masterFd) == 0);
    return true;
}

int32_t uCxAtClientHandleRxAvailable(uCxAtClient_t *pClient)
{
    if (!pClient->opened) {
        return -1;
    }
    if (atomic_load(&gRxFailures) > 0) {
        atomic_fetch_sub(&gRxFailures, 1);
        return -1;
    }
    uint8_t byte;
    int32_t result = uPortUartRead(pClient->uartHandle, &byte, 1, 0);
    if (result > 0) {
        atomic_fetch_add(&gRxCalls, 1);
    }
    return result < 0 ? result : 0;
}

static bool testPosixOsPrimitives(void)
{
    uPortInit();
    int32_t startMs = uPortGetTickTimeMs();
    CHECK(uPortSleepMs(20) == 0);
    CHECK(uPortGetTickTimeMs() - startMs >= 10);

    pthread_mutex_t mutex;
    CHECK(pthread_mutex_init(&mutex, NULL) == 0);
    CHECK(uPortMutexTryLock(&mutex, 0) == 0);
    CHECK(uPortMutexTryLock(&mutex, 20) == ETIMEDOUT);
    CHECK(pthread_mutex_unlock(&mutex) == 0);
    CHECK(uPortMutexTryLock(&mutex, 20) == 0);
    CHECK(pthread_mutex_unlock(&mutex) == 0);
    CHECK(pthread_mutex_destroy(&mutex) == 0);
    uPortDeinit();
    return true;
}

static bool testBackgroundRxWakeAndStop(void)
{
    uCxAtClient_t client = {0};
    int masterFd;
    char slaveName[128];
    uint8_t byte = 0x55;
    atomic_store(&gRxCalls, 0);
    atomic_store(&gRxFailures, 0);

    CHECK(createPty(&masterFd, slaveName, sizeof(slaveName)));
    client.uartHandle = uPortUartOpen(slaveName, 115200, false);
    CHECK(client.uartHandle != NULL);
    client.opened = true;
    uPortBgRxTaskCreate(&client);
    CHECK(sleepMs(30));
    CHECK(atomic_load(&gRxCalls) == 0);

    CHECK(write(masterFd, &byte, sizeof(byte)) == (ssize_t)sizeof(byte));
    CHECK(sleepMs(30));
    CHECK(atomic_load(&gRxCalls) == 1);

    CHECK(sleepMs(30));
    CHECK(atomic_load(&gRxCalls) == 1);

    atomic_store(&gRxFailures, 1);
    CHECK(write(masterFd, &byte, sizeof(byte)) == (ssize_t)sizeof(byte));
    CHECK(sleepMs(30));
    CHECK(atomic_load(&gRxFailures) == 0);

    uPortBgRxTaskDestroy(&client);
    uPortUartClose(client.uartHandle);
    CHECK(close(masterFd) == 0);
    return true;
}

static bool testBackgroundRxCreateFailure(void)
{
    uCxAtClient_t client = {0};
    int joinsBefore = atomic_load(&gThreadJoinCalls);

    client.opened = true;
    atomic_store(&gRxCalls, 0);
    atomic_store(&gFailThreadCreate, true);
    uPortBgRxTaskCreate(&client);
    CHECK(sleepMs(20));
    uPortBgRxTaskDestroy(&client);
    CHECK(atomic_load(&gRxCalls) == 0);
    CHECK(atomic_load(&gThreadJoinCalls) == joinsBefore);
    return true;
}

int main(void)
{
    static const struct {
        const char *pName;
        bool (*pTest)(void);
    } tests[] = {
        {"open and configuration", testOpenAndConfiguration},
        {"binary transfer and reopen", testBinaryTransferAndReopen},
        {"repeated open and close", testRepeatedOpenClose},
        {"read timeouts", testReadTimeouts},
        {"OS primitives", testPosixOsPrimitives},
        {"background RX wake and stop", testBackgroundRxWakeAndStop},
        {"background RX create failure", testBackgroundRxCreateFailure}
    };

    for (size_t index = 0; index < ARRAY_SIZE(tests); index++) {
        printf("[ RUN      ] %s\n", tests[index].pName);
        if (!tests[index].pTest()) {
            printf("[  FAILED  ] %s\n", tests[index].pName);
            return EXIT_FAILURE;
        }
        printf("[       OK ] %s\n", tests[index].pName);
    }

    printf("[  PASSED  ] %zu tests\n", ARRAY_SIZE(tests));
    return EXIT_SUCCESS;
}
