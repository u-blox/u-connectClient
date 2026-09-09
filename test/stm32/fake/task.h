#ifndef TASK_H
#define TASK_H

#include "FreeRTOS.h"

typedef struct fakeTask *TaskHandle_t;
typedef void (*TaskFunction_t)(void *);

typedef enum {
    eRunning,
    eReady,
    eBlocked,
    eSuspended,
    eDeleted,
    eInvalid
} eTaskState;

BaseType_t xTaskCreate(TaskFunction_t taskFunction,
                       const char *pName,
                       uint16_t stackDepth,
                       void *pParameter,
                       uint32_t priority,
                       TaskHandle_t *pTaskHandle);
TickType_t xTaskGetTickCount(void);
void vTaskDelay(TickType_t ticks);
void vTaskDelete(TaskHandle_t taskHandle);
eTaskState eTaskGetState(TaskHandle_t taskHandle);
uint32_t ulTaskNotifyTake(BaseType_t clearOnExit, TickType_t ticksToWait);
BaseType_t xTaskNotifyGive(TaskHandle_t taskHandle);
void vTaskNotifyGiveFromISR(TaskHandle_t taskHandle,
                            BaseType_t *pHigherPriorityTaskWoken);

#endif
