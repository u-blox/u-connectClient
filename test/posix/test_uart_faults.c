#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pty.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "u_port_uart.h"

#define CHECK(condition)                                                        \
    do {                                                                        \
        if (!(condition)) {                                                     \
            fprintf(stderr, "%s:%d: check failed: %s\n",                     \
                    __FILE__, __LINE__, #condition);                            \
            return false;                                                       \
        }                                                                       \
    } while (0)

typedef enum {
    FAULT_NONE,
    FAULT_MALLOC,
    FAULT_TCGETATTR,
    FAULT_TCSETATTR,
    FAULT_POLL_EINTR,
    FAULT_POLL_ERROR,
    FAULT_READ_EINTR,
    FAULT_READ_ERROR,
    FAULT_WRITE_EINTR,
    FAULT_WRITE_ERROR,
    FAULT_WRITE_SHORT,
    FAULT_WRITE_ZERO
} fault_t;

static fault_t gFault;

void *__real_malloc(size_t size);
int __real_tcgetattr(int fd, struct termios *pSettings);
int __real_tcsetattr(int fd, int actions, const struct termios *pSettings);
int __real_poll(struct pollfd *pFds, nfds_t count, int timeout);
ssize_t __real_read(int fd, void *pData, size_t length);
ssize_t __real_write(int fd, const void *pData, size_t length);

void *__wrap_malloc(size_t size)
{
    if (gFault == FAULT_MALLOC) {
        gFault = FAULT_NONE;
        return NULL;
    }
    return __real_malloc(size);
}

int __wrap_tcgetattr(int fd, struct termios *pSettings)
{
    if (gFault == FAULT_TCGETATTR) {
        gFault = FAULT_NONE;
        errno = EIO;
        return -1;
    }
    return __real_tcgetattr(fd, pSettings);
}

int __wrap_tcsetattr(int fd, int actions, const struct termios *pSettings)
{
    if (gFault == FAULT_TCSETATTR) {
        gFault = FAULT_NONE;
        errno = EIO;
        return -1;
    }
    return __real_tcsetattr(fd, actions, pSettings);
}

int __wrap_poll(struct pollfd *pFds, nfds_t count, int timeout)
{
    if (gFault == FAULT_POLL_EINTR) {
        gFault = FAULT_NONE;
        errno = EINTR;
        return -1;
    }
    if (gFault == FAULT_POLL_ERROR) {
        gFault = FAULT_NONE;
        errno = EIO;
        return -1;
    }
    return __real_poll(pFds, count, timeout);
}

ssize_t __wrap_read(int fd, void *pData, size_t length)
{
    if (gFault == FAULT_READ_EINTR) {
        gFault = FAULT_NONE;
        errno = EINTR;
        return -1;
    }
    if (gFault == FAULT_READ_ERROR) {
        gFault = FAULT_NONE;
        errno = EIO;
        return -1;
    }
    return __real_read(fd, pData, length);
}

ssize_t __wrap_write(int fd, const void *pData, size_t length)
{
    if (gFault == FAULT_WRITE_EINTR) {
        gFault = FAULT_NONE;
        errno = EINTR;
        return -1;
    }
    if (gFault == FAULT_WRITE_ERROR) {
        gFault = FAULT_NONE;
        errno = EIO;
        return -1;
    }
    if (gFault == FAULT_WRITE_SHORT) {
        gFault = FAULT_NONE;
        length = length > 2 ? 2 : length;
    } else if (gFault == FAULT_WRITE_ZERO) {
        gFault = FAULT_NONE;
        return 0;
    }
    return __real_write(fd, pData, length);
}

static bool createPty(int *pMasterFd, char *pSlaveName)
{
    int slaveFd;
    CHECK(openpty(pMasterFd, &slaveFd, pSlaveName, NULL, NULL) == 0);
    CHECK(close(slaveFd) == 0);
    return true;
}

static bool testOpenFailures(void)
{
    char slaveName[128];
    int masterFd;
    CHECK(createPty(&masterFd, slaveName));

    gFault = FAULT_MALLOC;
    CHECK(uPortUartOpen(slaveName, 115200, false) == NULL);
    gFault = FAULT_TCGETATTR;
    CHECK(uPortUartOpen(slaveName, 115200, false) == NULL);
    gFault = FAULT_TCSETATTR;
    CHECK(uPortUartOpen(slaveName, 115200, false) == NULL);

    CHECK(close(masterFd) == 0);
    return true;
}

static bool testReadFailures(void)
{
    static const uint8_t expected = 0x5a;
    uint8_t received = 0;
    char slaveName[128];
    int masterFd;
    CHECK(createPty(&masterFd, slaveName));
    uPortUartHandle_t handle = uPortUartOpen(slaveName, 115200, false);
    CHECK(handle != NULL);

    CHECK(write(masterFd, &expected, 1) == 1);
    gFault = FAULT_POLL_EINTR;
    CHECK(uPortUartRead(handle, &received, 1, 100) == 1);
    CHECK(received == expected);

    gFault = FAULT_POLL_ERROR;
    CHECK(uPortUartRead(handle, &received, 1, 0) < 0);

    CHECK(write(masterFd, &expected, 1) == 1);
    gFault = FAULT_READ_EINTR;
    CHECK(uPortUartRead(handle, &received, 1, 100) == 1);
    CHECK(received == expected);

    CHECK(write(masterFd, &expected, 1) == 1);
    gFault = FAULT_READ_ERROR;
    CHECK(uPortUartRead(handle, &received, 1, 100) < 0);

    CHECK(close(masterFd) == 0);
    CHECK(uPortUartRead(handle, &received, 1, 0) < 0);
    uPortUartClose(handle);
    return true;
}

static bool testWriteFailures(void)
{
    static const uint8_t expected[] = {1, 2, 3, 4};
    uint8_t received[sizeof(expected)];
    char slaveName[128];
    int masterFd;
    CHECK(createPty(&masterFd, slaveName));
    uPortUartHandle_t handle = uPortUartOpen(slaveName, 115200, false);
    CHECK(handle != NULL);

    gFault = FAULT_WRITE_EINTR;
    CHECK(uPortUartWrite(handle, expected, sizeof(expected)) ==
          (int32_t)sizeof(expected));
    CHECK(read(masterFd, received, sizeof(received)) == (ssize_t)sizeof(received));

    gFault = FAULT_WRITE_SHORT;
    CHECK(uPortUartWrite(handle, expected, sizeof(expected)) ==
          (int32_t)sizeof(expected));
    CHECK(read(masterFd, received, sizeof(received)) == (ssize_t)sizeof(received));

    gFault = FAULT_WRITE_ERROR;
    CHECK(uPortUartWrite(handle, expected, sizeof(expected)) < 0);
    gFault = FAULT_WRITE_ZERO;
    CHECK(uPortUartWrite(handle, expected, sizeof(expected)) < 0);

    uPortUartClose(handle);
    CHECK(close(masterFd) == 0);
    return true;
}

int main(void)
{
    if (!testOpenFailures() || !testReadFailures() || !testWriteFailures()) {
        return EXIT_FAILURE;
    }
    printf("[  PASSED  ] UART fault injection\n");
    return EXIT_SUCCESS;
}
