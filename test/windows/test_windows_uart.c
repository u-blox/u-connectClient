#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fake_windows.h"
#include "u_port_uart.h"

#define CHECK(condition)                                                        \
    do {                                                                        \
        if (!(condition)) {                                                     \
            fprintf(stderr, "%s:%d: check failed: %s\n",                     \
                    __FILE__, __LINE__, #condition);                            \
            return false;                                                       \
        }                                                                       \
    } while (0)

static bool testOpenConfiguration(void)
{
    fakeWindowsReset();
    uPortUartHandle_t handle = uPortUartOpen("COM12", 921600, true);
    CHECK(handle != NULL);
    CHECK(strcmp(gFakeWindows.openedName, "\\\\.\\COM12") == 0);
    CHECK(gFakeWindows.dcb.DCBlength == sizeof(DCB));
    CHECK(gFakeWindows.dcb.BaudRate == 921600);
    CHECK(gFakeWindows.dcb.ByteSize == 8);
    CHECK(gFakeWindows.dcb.fRtsControl == RTS_CONTROL_HANDSHAKE);
    CHECK(gFakeWindows.dcb.fOutxCtsFlow == TRUE);
    CHECK(gFakeWindows.timeouts.ReadTotalTimeoutConstant == 100);
    CHECK(gFakeWindows.timeouts.WriteTotalTimeoutConstant == 1000);
    CHECK(gFakeWindows.commMask == EV_RXCHAR);
    CHECK(gFakeWindows.purgeFlags == (PURGE_RXCLEAR | PURGE_TXCLEAR));
    uPortUartClose(handle);

    fakeWindowsReset();
    handle = uPortUartOpen("pipe-name", 115200, false);
    CHECK(handle != NULL);
    CHECK(strcmp(gFakeWindows.openedName, "pipe-name") == 0);
    CHECK(gFakeWindows.dcb.fRtsControl == RTS_CONTROL_ENABLE);
    CHECK(gFakeWindows.dcb.fOutxCtsFlow == FALSE);
    uPortUartClose(handle);

    fakeWindowsReset();
    gFakeWindows.setupCommResult = FALSE;
    handle = uPortUartOpen("COM2", 115200, false);
    CHECK(handle != NULL);
    uPortUartClose(handle);
    return true;
}

static bool testOpenFailures(void)
{
    CHECK(uPortUartOpen(NULL, 115200, false) == NULL);

    fakeWindowsReset();
    gFakeWindows.createFileResult = FALSE;
    CHECK(uPortUartOpen("COM1", 115200, false) == NULL);

    fakeWindowsReset();
    gFakeWindows.getCommStateResult = FALSE;
    CHECK(uPortUartOpen("COM1", 115200, false) == NULL);
    CHECK(gFakeWindows.closeCalls == 1);

    fakeWindowsReset();
    gFakeWindows.setCommStateResult = FALSE;
    CHECK(uPortUartOpen("COM1", 115200, false) == NULL);
    CHECK(gFakeWindows.closeCalls == 1);

    fakeWindowsReset();
    gFakeWindows.setCommTimeoutsResult = FALSE;
    CHECK(uPortUartOpen("COM1", 115200, false) == NULL);
    CHECK(gFakeWindows.closeCalls == 1);

    fakeWindowsReset();
    gFakeWindows.setCommMaskResult = FALSE;
    CHECK(uPortUartOpen("COM1", 115200, false) == NULL);
    CHECK(gFakeWindows.closeCalls == 1);
    return true;
}

static bool testReadWriteAndEvents(void)
{
    uint8_t data[8] = {0};
    fakeWindowsReset();
    uPortUartHandle_t handle = uPortUartOpen("COM1", 115200, false);
    CHECK(handle != NULL);

    gFakeWindows.queuedBytes = 3;
    gFakeWindows.bytesRead = 3;
    CHECK(uPortUartRead(handle, data, sizeof(data), 0) == 3);
    CHECK(gFakeWindows.readLength == 3);
    gFakeWindows.queuedBytes = 8;
    gFakeWindows.bytesRead = 2;
    CHECK(uPortUartRead(handle, data, sizeof(data), 0) == 2);
    CHECK(gFakeWindows.readLength == sizeof(data));
    gFakeWindows.queuedBytes = 0;
    CHECK(uPortUartRead(handle, data, sizeof(data), 0) == 0);
    gFakeWindows.clearCommErrorResult = FALSE;
    CHECK(uPortUartRead(handle, data, sizeof(data), 0) < 0);

    gFakeWindows.bytesWritten = 5;
    CHECK(uPortUartWrite(handle, data, 5) == 5);
    CHECK(gFakeWindows.writeLength == 5);
    gFakeWindows.bytesWritten = 2;
    CHECK(uPortUartWrite(handle, data, 5) == 2);
    gFakeWindows.writeResult = FALSE;
    CHECK(uPortUartWrite(handle, data, 5) < 0);

    gFakeWindows.waitCommCalls = 0;
    CHECK(uPortUartWaitForData(handle, -1) == 1);
    gFakeWindows.waitCommCalls = 0;
    gFakeWindows.waitEventMask = 0;
    CHECK(uPortUartWaitForData(handle, -1) == 0);
    gFakeWindows.waitCommCalls = 1;
    gFakeWindows.lastError = ERROR_OPERATION_ABORTED;
    CHECK(uPortUartWaitForData(handle, -1) == 0);
    uPortUartWake(handle);
    CHECK(gFakeWindows.cancelCalls == 1);

    CHECK(uPortUartRead(NULL, data, sizeof(data), 0) < 0);
    CHECK(uPortUartRead(handle, NULL, sizeof(data), 0) == 0);
    CHECK(uPortUartRead(handle, data, 0, 0) < 0);
    CHECK(uPortUartWrite(NULL, data, sizeof(data)) < 0);
    CHECK(uPortUartWrite(handle, data, 0) < 0);
    CHECK(uPortUartWaitForData(NULL, 0) < 0);
    uPortUartClose(handle);
    uPortUartClose(NULL);
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
        {"read write and events", testReadWriteAndEvents}
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