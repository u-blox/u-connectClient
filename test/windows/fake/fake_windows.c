#include <string.h>

#include "fake_windows.h"

static int gComHandle;
static int gThreadHandle;
static int gSemaphoreHandle;
static LPTHREAD_START_ROUTINE gpThreadStart;
static LPVOID gpThreadParameter;

fakeWindowsState_t gFakeWindows;

void fakeWindowsReset(void)
{
    memset(&gFakeWindows, 0, sizeof(gFakeWindows));
    gFakeWindows.createFileResult = TRUE;
    gFakeWindows.setupCommResult = TRUE;
    gFakeWindows.getCommStateResult = TRUE;
    gFakeWindows.setCommStateResult = TRUE;
    gFakeWindows.setCommTimeoutsResult = TRUE;
    gFakeWindows.setCommMaskResult = TRUE;
    gFakeWindows.readResult = TRUE;
    gFakeWindows.writeResult = TRUE;
    gFakeWindows.clearCommErrorResult = TRUE;
    gFakeWindows.createThreadResult = TRUE;
    gFakeWindows.waitResult = WAIT_OBJECT_0;
    gFakeWindows.waitEventMask = EV_RXCHAR;
    gFakeWindows.performanceFrequency = 1000;
    gFakeWindows.dcb.DCBlength = sizeof(DCB);
    gpThreadStart = NULL;
    gpThreadParameter = NULL;
}

void fakeWindowsRunThread(void)
{
    if (gpThreadStart != NULL) {
        LPTHREAD_START_ROUTINE pStart = gpThreadStart;
        gpThreadStart = NULL;
        pStart(gpThreadParameter);
    }
}

HANDLE CreateFileA(LPCSTR pName, DWORD access, DWORD shareMode,
                   LPSECURITY_ATTRIBUTES pSecurity, DWORD creation,
                   DWORD flags, HANDLE templateFile)
{
    (void)access;
    (void)shareMode;
    (void)pSecurity;
    (void)creation;
    (void)flags;
    (void)templateFile;
    size_t nameLength = strlen(pName);
    if (nameLength >= sizeof(gFakeWindows.openedName)) {
        nameLength = sizeof(gFakeWindows.openedName) - 1;
    }
    memcpy(gFakeWindows.openedName, pName, nameLength);
    gFakeWindows.openedName[nameLength] = 0;
    return gFakeWindows.createFileResult ? &gComHandle : INVALID_HANDLE_VALUE;
}

BOOL SetupComm(HANDLE handle, DWORD inputSize, DWORD outputSize)
{
    (void)handle;
    return gFakeWindows.setupCommResult && inputSize == 16384 &&
           outputSize == 16384;
}

BOOL GetCommState(HANDLE handle, DCB *pDcb)
{
    (void)handle;
    if (gFakeWindows.getCommStateResult) {
        *pDcb = gFakeWindows.dcb;
    }
    return gFakeWindows.getCommStateResult;
}

BOOL SetCommState(HANDLE handle, DCB *pDcb)
{
    (void)handle;
    gFakeWindows.dcb = *pDcb;
    return gFakeWindows.setCommStateResult;
}

BOOL SetCommTimeouts(HANDLE handle, COMMTIMEOUTS *pTimeouts)
{
    (void)handle;
    gFakeWindows.timeouts = *pTimeouts;
    return gFakeWindows.setCommTimeoutsResult;
}

BOOL SetCommMask(HANDLE handle, DWORD eventMask)
{
    (void)handle;
    gFakeWindows.commMask = eventMask;
    return gFakeWindows.setCommMaskResult;
}

BOOL PurgeComm(HANDLE handle, DWORD flags)
{
    (void)handle;
    gFakeWindows.purgeFlags = flags;
    return TRUE;
}

BOOL WriteFile(HANDLE handle, const void *pData, DWORD length,
               LPDWORD pWritten, LPOVERLAPPED pOverlapped)
{
    (void)handle;
    (void)pData;
    (void)pOverlapped;
    gFakeWindows.writeLength = length;
    *pWritten = gFakeWindows.bytesWritten;
    return gFakeWindows.writeResult;
}

BOOL ReadFile(HANDLE handle, void *pData, DWORD length,
              LPDWORD pRead, LPOVERLAPPED pOverlapped)
{
    (void)handle;
    (void)pData;
    (void)pOverlapped;
    gFakeWindows.readLength = length;
    *pRead = gFakeWindows.bytesRead;
    return gFakeWindows.readResult;
}

BOOL ClearCommError(HANDLE handle, LPDWORD pErrors, COMSTAT *pStatus)
{
    (void)handle;
    *pErrors = 0;
    pStatus->cbInQue = gFakeWindows.queuedBytes;
    return gFakeWindows.clearCommErrorResult;
}

BOOL WaitCommEvent(HANDLE handle, LPDWORD pEventMask,
                   LPOVERLAPPED pOverlapped)
{
    (void)handle;
    (void)pOverlapped;
    gFakeWindows.waitCommCalls++;
    if (gFakeWindows.waitCommCalls == 1) {
        *pEventMask = gFakeWindows.waitEventMask;
        return TRUE;
    }
    if (gFakeWindows.lastError == 0) {
        gFakeWindows.lastError = ERROR_ACCESS_DENIED;
    }
    return FALSE;
}

BOOL CancelIoEx(HANDLE handle, LPOVERLAPPED pOverlapped)
{
    (void)handle;
    (void)pOverlapped;
    gFakeWindows.cancelCalls++;
    return TRUE;
}

DWORD GetLastError(void)
{
    return gFakeWindows.lastError;
}

BOOL CloseHandle(HANDLE handle)
{
    (void)handle;
    gFakeWindows.closeCalls++;
    return TRUE;
}

BOOL QueryPerformanceFrequency(LARGE_INTEGER *pFrequency)
{
    pFrequency->QuadPart = gFakeWindows.performanceFrequency;
    return TRUE;
}

BOOL QueryPerformanceCounter(LARGE_INTEGER *pCounter)
{
    pCounter->QuadPart = gFakeWindows.performanceCounter;
    return TRUE;
}

DWORD WaitForSingleObject(HANDLE handle, DWORD timeout)
{
    (void)handle;
    gFakeWindows.lastWaitTimeout = timeout;
    return gFakeWindows.waitResult;
}

HANDLE CreateSemaphore(LPSECURITY_ATTRIBUTES pAttributes, LONG initialCount,
                       LONG maximumCount, LPCSTR pName)
{
    (void)pAttributes;
    (void)initialCount;
    (void)maximumCount;
    (void)pName;
    return &gSemaphoreHandle;
}

BOOL ReleaseSemaphore(HANDLE handle, LONG releaseCount,
                      LONG *pPreviousCount)
{
    (void)handle;
    (void)releaseCount;
    (void)pPreviousCount;
    return TRUE;
}

HANDLE CreateThread(LPSECURITY_ATTRIBUTES pAttributes, size_t stackSize,
                    LPTHREAD_START_ROUTINE pStart, LPVOID pParameter,
                    DWORD flags, LPDWORD pThreadId)
{
    (void)pAttributes;
    (void)stackSize;
    (void)flags;
    (void)pThreadId;
    if (!gFakeWindows.createThreadResult) {
        return NULL;
    }
    gpThreadStart = pStart;
    gpThreadParameter = pParameter;
    return &gThreadHandle;
}

void Sleep(DWORD milliseconds)
{
    (void)milliseconds;
}