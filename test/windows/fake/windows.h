#ifndef FAKE_WINDOWS_H
#define FAKE_WINDOWS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int BOOL;
typedef uint8_t BYTE;
typedef unsigned long DWORD;
typedef int32_t LONG;
typedef void *HANDLE;
typedef void *LPVOID;
typedef const char *LPCSTR;
typedef DWORD *LPDWORD;
typedef void *LPSECURITY_ATTRIBUTES;
typedef void *LPOVERLAPPED;
typedef DWORD (*LPTHREAD_START_ROUTINE)(LPVOID);

typedef struct {
    int64_t QuadPart;
} LARGE_INTEGER;

typedef struct {
    DWORD DCBlength;
    DWORD BaudRate;
    DWORD fBinary;
    DWORD fOutxCtsFlow;
    DWORD fOutxDsrFlow;
    DWORD fDtrControl;
    DWORD fDsrSensitivity;
    DWORD fOutX;
    DWORD fInX;
    DWORD fErrorChar;
    DWORD fNull;
    DWORD fRtsControl;
    DWORD fAbortOnError;
    BYTE ByteSize;
    BYTE Parity;
    BYTE StopBits;
} DCB;

typedef struct {
    DWORD ReadIntervalTimeout;
    DWORD ReadTotalTimeoutMultiplier;
    DWORD ReadTotalTimeoutConstant;
    DWORD WriteTotalTimeoutMultiplier;
    DWORD WriteTotalTimeoutConstant;
} COMMTIMEOUTS;

typedef struct {
    DWORD fFlags;
    DWORD cbInQue;
    DWORD cbOutQue;
} COMSTAT;

#define WINAPI
#define TRUE 1
#define FALSE 0
#define NULL_HANDLE ((HANDLE)0)
#define INVALID_HANDLE_VALUE ((HANDLE)(intptr_t)-1)
#define GENERIC_READ 0x80000000UL
#define GENERIC_WRITE 0x40000000UL
#define OPEN_EXISTING 3UL
#define NOPARITY 0U
#define ONESTOPBIT 0U
#define RTS_CONTROL_ENABLE 1UL
#define RTS_CONTROL_HANDSHAKE 2UL
#define DTR_CONTROL_ENABLE 1UL
#define EV_RXCHAR 1UL
#define PURGE_TXCLEAR 4UL
#define PURGE_RXCLEAR 8UL
#define ERROR_ACCESS_DENIED 5UL
#define ERROR_OPERATION_ABORTED 995UL
#define WAIT_OBJECT_0 0UL
#define WAIT_TIMEOUT 258UL
#define WAIT_FAILED 0xffffffffUL
#define INFINITE 0xffffffffUL

HANDLE CreateFileA(LPCSTR pName, DWORD access, DWORD shareMode,
                   LPSECURITY_ATTRIBUTES pSecurity, DWORD creation,
                   DWORD flags, HANDLE templateFile);
BOOL SetupComm(HANDLE handle, DWORD inputSize, DWORD outputSize);
BOOL GetCommState(HANDLE handle, DCB *pDcb);
BOOL SetCommState(HANDLE handle, DCB *pDcb);
BOOL SetCommTimeouts(HANDLE handle, COMMTIMEOUTS *pTimeouts);
BOOL SetCommMask(HANDLE handle, DWORD eventMask);
BOOL PurgeComm(HANDLE handle, DWORD flags);
BOOL WriteFile(HANDLE handle, const void *pData, DWORD length,
               LPDWORD pWritten, LPOVERLAPPED pOverlapped);
BOOL ReadFile(HANDLE handle, void *pData, DWORD length,
              LPDWORD pRead, LPOVERLAPPED pOverlapped);
BOOL ClearCommError(HANDLE handle, LPDWORD pErrors, COMSTAT *pStatus);
BOOL WaitCommEvent(HANDLE handle, LPDWORD pEventMask,
                   LPOVERLAPPED pOverlapped);
BOOL CancelIoEx(HANDLE handle, LPOVERLAPPED pOverlapped);
DWORD GetLastError(void);
BOOL CloseHandle(HANDLE handle);
BOOL QueryPerformanceFrequency(LARGE_INTEGER *pFrequency);
BOOL QueryPerformanceCounter(LARGE_INTEGER *pCounter);
DWORD WaitForSingleObject(HANDLE handle, DWORD timeout);
HANDLE CreateSemaphore(LPSECURITY_ATTRIBUTES pAttributes, LONG initialCount,
                       LONG maximumCount, LPCSTR pName);
BOOL ReleaseSemaphore(HANDLE handle, LONG releaseCount,
                      LONG *pPreviousCount);
HANDLE CreateThread(LPSECURITY_ATTRIBUTES pAttributes, size_t stackSize,
                    LPTHREAD_START_ROUTINE pStart, LPVOID pParameter,
                    DWORD flags, LPDWORD pThreadId);
void Sleep(DWORD milliseconds);

#ifdef __cplusplus
}
#endif

#endif