#ifndef FAKE_WINDOWS_API_H
#define FAKE_WINDOWS_API_H

#include "windows.h"

typedef struct {
    BOOL createFileResult;
    BOOL setupCommResult;
    BOOL getCommStateResult;
    BOOL setCommStateResult;
    BOOL setCommTimeoutsResult;
    BOOL setCommMaskResult;
    BOOL readResult;
    BOOL writeResult;
    BOOL clearCommErrorResult;
    BOOL createThreadResult;
    DWORD lastError;
    DWORD waitResult;
    DWORD waitEventMask;
    DWORD queuedBytes;
    DWORD bytesRead;
    DWORD bytesWritten;
    int64_t performanceCounter;
    int64_t performanceFrequency;
    char openedName[64];
    DCB dcb;
    COMMTIMEOUTS timeouts;
    DWORD commMask;
    DWORD purgeFlags;
    DWORD lastWaitTimeout;
    DWORD readLength;
    DWORD writeLength;
    unsigned closeCalls;
    unsigned cancelCalls;
    unsigned waitCommCalls;
    unsigned rxCalls;
} fakeWindowsState_t;

extern fakeWindowsState_t gFakeWindows;

void fakeWindowsReset(void);
void fakeWindowsRunThread(void);

#endif