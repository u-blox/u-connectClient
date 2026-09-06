#ifndef FAKE_FREERTOS_H
#define FAKE_FREERTOS_H

#include <stdbool.h>
#include <stdint.h>

#include "task.h"

void fakeRtosReset(void);
void fakeRtosSetTickCount(TickType_t ticks);
void fakeRtosSetSemaphoreResult(BaseType_t result);
void fakeRtosSetTaskCreateResult(BaseType_t result);
void fakeRtosJoinTask(void);
uint32_t fakeRtosGetDelayCalls(void);
TickType_t fakeRtosGetLastDelay(void);
TickType_t fakeRtosGetLastSemaphoreTimeout(void);
uint16_t fakeRtosGetTaskStackDepth(void);
uint32_t fakeRtosGetTaskPriority(void);
const char *fakeRtosGetTaskName(void);
uint32_t fakeRtosGetTaskCreateCalls(void);
bool fakeRtosGetStateCalledWithNull(void);

#endif
