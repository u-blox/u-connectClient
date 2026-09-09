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
 * @brief Linux UART port implementation using termios.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <termios.h>

#include "u_port_uart.h"

/* ----------------------------------------------------------------
 * COMPILE-TIME MACROS
 * -------------------------------------------------------------- */

/* ----------------------------------------------------------------
 * TYPES
 * -------------------------------------------------------------- */

/** Structure representing a UART handle.
 */
typedef struct {
    int fd;  /**< File descriptor for the UART device */
    int wakeReadFd;
    int wakeWriteFd;
} uPortUartHandle;

/* ----------------------------------------------------------------
 * STATIC FUNCTION PROTOTYPES
 * -------------------------------------------------------------- */

/* ----------------------------------------------------------------
 * PUBLIC FUNCTIONS
 * -------------------------------------------------------------- */

uPortUartHandle_t uPortUartOpen(const char *pDevice, int32_t baudRate, bool useFlowControl)
{
    if (pDevice == NULL) {
        return NULL;
    }

    uPortUartHandle *pHandle = (uPortUartHandle *)malloc(sizeof(uPortUartHandle));
    if (pHandle == NULL) {
        return NULL;
    }
    pHandle->fd = -1;
    pHandle->wakeReadFd = -1;
    pHandle->wakeWriteFd = -1;

    int wakePipe[2];
    if (pipe(wakePipe) != 0) {
        free(pHandle);
        return NULL;
    }
    pHandle->wakeReadFd = wakePipe[0];
    pHandle->wakeWriteFd = wakePipe[1];
    (void)fcntl(pHandle->wakeReadFd, F_SETFL, O_NONBLOCK);
    (void)fcntl(pHandle->wakeWriteFd, F_SETFL, O_NONBLOCK);

    // Open the UART device
    pHandle->fd = open(pDevice, O_RDWR | O_NOCTTY);
    if (pHandle->fd < 0) {
        close(pHandle->wakeReadFd);
        close(pHandle->wakeWriteFd);
        free(pHandle);
        return NULL;
    }

    // Configure the UART
    struct termios tty;
    if (tcgetattr(pHandle->fd, &tty) != 0) {
        close(pHandle->fd);
        close(pHandle->wakeReadFd);
        close(pHandle->wakeWriteFd);
        free(pHandle);
        return NULL;
    }

    // Set baud rate
    speed_t speed;
    switch (baudRate) {
        case 9600:
            speed = B9600;
            break;
        case 19200:
            speed = B19200;
            break;
        case 38400:
            speed = B38400;
            break;
        case 57600:
            speed = B57600;
            break;
        case 115200:
            speed = B115200;
            break;
        case 230400:
            speed = B230400;
            break;
        case 460800:
            speed = B460800;
            break;
        case 921600:
            speed = B921600;
            break;
        case 1000000:
            speed = B1000000;
            break;
        case 1500000:
            speed = B1500000;
            break;
        case 2000000:
            speed = B2000000;
            break;
        case 3000000:
            speed = B3000000;
            break;
        default:
            close(pHandle->fd);
            close(pHandle->wakeReadFd);
            close(pHandle->wakeWriteFd);
            free(pHandle);
            return NULL;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    // 8N1 mode
    tty.c_cflag &= (unsigned int)~PARENB;  // No parity
    tty.c_cflag &= (unsigned int)~CSTOPB;  // 1 stop bit
    tty.c_cflag &= (unsigned int)~CSIZE;
    tty.c_cflag |= CS8;  // 8 bits

    // Configure hardware flow control
    if (useFlowControl) {
        tty.c_cflag |= CRTSCTS;
    } else {
        tty.c_cflag &= (unsigned int)~CRTSCTS;
    }

    // Enable reading
    tty.c_cflag |= CREAD | CLOCAL;

    // Raw mode
    tty.c_lflag &= (unsigned int)~(ICANON | ECHO | ECHOE | ISIG);
    // Also disable CR/NL translation, otherwise 0x0D/0x0A bytes inside binary payloads get corrupted
    tty.c_iflag &= (unsigned int)~(IXON | IXOFF | IXANY | ICRNL | INLCR | IGNCR | ISTRIP);
    tty.c_oflag &= (unsigned int)~OPOST;

    // Non-blocking reads with timeout handled by poll
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(pHandle->fd, TCSANOW, &tty) != 0) {
        close(pHandle->fd);
        close(pHandle->wakeReadFd);
        close(pHandle->wakeWriteFd);
        free(pHandle);
        return NULL;
    }

    return (uPortUartHandle_t)pHandle;
}

void uPortUartClose(uPortUartHandle_t handle)
{
    if (handle != NULL) {
        uPortUartHandle *pHandle = (uPortUartHandle *)handle;
        close(pHandle->fd);
        close(pHandle->wakeReadFd);
        close(pHandle->wakeWriteFd);
        free(pHandle);
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
    const uint8_t *pBytes = (const uint8_t *)pData;
    size_t totalWritten = 0;

    while (totalWritten < length) {
        ssize_t written = write(pHandle->fd, pBytes + totalWritten, length - totalWritten);
        if (written < 0) {
            if (errno == EINTR) {
                continue;  // Interrupted, try again
            }
            return -1;
        }
        if (written == 0) {
            return -1;
        }
        totalWritten += (size_t)written;
    }

    return (int32_t)totalWritten;
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

    // If pData is NULL, just return 0 (test case)
    if (pData == NULL) {
        return 0;
    }

    struct pollfd pollFd = {
        .fd = pHandle->fd,
        .events = POLLIN
    };
    int pollResult;
    do {
        pollResult = poll(&pollFd, 1, timeoutMs);
    } while ((pollResult < 0) && (errno == EINTR));

    if (pollResult == 0) {
        return 0;
    }
    if ((pollResult < 0) || ((pollFd.revents & (POLLERR | POLLHUP | POLLNVAL)) != 0)) {
        return -1;
    }

    ssize_t bytesRead;
    do {
        bytesRead = read(pHandle->fd, pData, length);
    } while ((bytesRead < 0) && (errno == EINTR));

    if (bytesRead < 0) {
        return -1;
    }

    return (int32_t)bytesRead;
}

int32_t uPortUartWaitForData(uPortUartHandle_t handle, int32_t timeoutMs)
{
    if (handle == NULL) {
        return -1;
    }

    uPortUartHandle *pHandle = (uPortUartHandle *)handle;
    struct pollfd pollFds[] = {
        {.fd = pHandle->fd, .events = POLLIN},
        {.fd = pHandle->wakeReadFd, .events = POLLIN}
    };
    int result;
    do {
        result = poll(pollFds, 2, timeoutMs);
    } while ((result < 0) && (errno == EINTR));

    if (result <= 0) {
        return result;
    }
    if ((pollFds[1].revents & POLLIN) != 0) {
        uint8_t wakeData[16];
        while (read(pHandle->wakeReadFd, wakeData, sizeof(wakeData)) > 0) {
        }
        return 0;
    }
    if ((pollFds[0].revents & POLLIN) != 0) {
        return 1;
    }
    return -1;
}

void uPortUartWake(uPortUartHandle_t handle)
{
    if (handle != NULL) {
        uPortUartHandle *pHandle = (uPortUartHandle *)handle;
        uint8_t wakeByte = 1;
        (void)write(pHandle->wakeWriteFd, &wakeByte, sizeof(wakeByte));
    }
}
