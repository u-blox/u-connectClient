#include <pthread.h>
#include <stdatomic.h>
#include <string.h>
#include <time.h>

#include "fake_freertos.h"
#include "semphr.h"

struct fakeTask {
    pthread_t thread;
    atomic_int state;
    TaskFunction_t taskFunction;
    void *pParameter;
    bool threadCreated;
};

static struct fakeTask gTask;
static atomic_uint gTickCount;
static atomic_uint gDelayCalls;
static TickType_t gLastDelay;
static TickType_t gLastSemaphoreTimeout;
static BaseType_t gSemaphoreResult;
static BaseType_t gTaskCreateResult;
static uint16_t gTaskStackDepth;
static uint32_t gTaskPriority;
static uint32_t gTaskCreateCalls;
static const char *gpTaskName;
static bool gStateCalledWithNull;
static pthread_mutex_t gNotifyMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t gNotifyCondition = PTHREAD_COND_INITIALIZER;
static uint32_t gNotificationCount;

static void *taskEntry(void *pParameter)
{
    struct fakeTask *pTask = pParameter;
    pTask->taskFunction(pTask->pParameter);
    atomic_store(&pTask->state, eDeleted);
    return NULL;
}

void fakeRtosReset(void)
{
    memset(&gTask, 0, sizeof(gTask));
    atomic_store(&gTask.state, eDeleted);
    atomic_store(&gTickCount, 0);
    atomic_store(&gDelayCalls, 0);
    gLastDelay = 0;
    gLastSemaphoreTimeout = 0;
    gSemaphoreResult = pdTRUE;
    gTaskCreateResult = pdPASS;
    gTaskStackDepth = 0;
    gTaskPriority = 0;
    gTaskCreateCalls = 0;
    gpTaskName = NULL;
    gStateCalledWithNull = false;
    pthread_mutex_lock(&gNotifyMutex);
    gNotificationCount = 0;
    pthread_mutex_unlock(&gNotifyMutex);
}

void fakeRtosSetTickCount(TickType_t ticks)
{
    atomic_store(&gTickCount, ticks);
}

void fakeRtosSetSemaphoreResult(BaseType_t result)
{
    gSemaphoreResult = result;
}

void fakeRtosSetTaskCreateResult(BaseType_t result)
{
    gTaskCreateResult = result;
}

void fakeRtosJoinTask(void)
{
    if (gTask.threadCreated) {
        pthread_join(gTask.thread, NULL);
        gTask.threadCreated = false;
    }
}

uint32_t fakeRtosGetDelayCalls(void)
{
    return atomic_load(&gDelayCalls);
}

TickType_t fakeRtosGetLastDelay(void)
{
    return gLastDelay;
}

TickType_t fakeRtosGetLastSemaphoreTimeout(void)
{
    return gLastSemaphoreTimeout;
}

uint16_t fakeRtosGetTaskStackDepth(void)
{
    return gTaskStackDepth;
}

uint32_t fakeRtosGetTaskPriority(void)
{
    return gTaskPriority;
}

const char *fakeRtosGetTaskName(void)
{
    return gpTaskName;
}

uint32_t fakeRtosGetTaskCreateCalls(void)
{
    return gTaskCreateCalls;
}

bool fakeRtosGetStateCalledWithNull(void)
{
    return gStateCalledWithNull;
}

BaseType_t xTaskCreate(TaskFunction_t taskFunction,
                       const char *pName,
                       uint16_t stackDepth,
                       void *pParameter,
                       uint32_t priority,
                       TaskHandle_t *pTaskHandle)
{
    gTaskCreateCalls++;
    gTaskStackDepth = stackDepth;
    gTaskPriority = priority;
    gpTaskName = pName;
    if (gTaskCreateResult != pdPASS) {
        return gTaskCreateResult;
    }

    gTask.taskFunction = taskFunction;
    gTask.pParameter = pParameter;
    atomic_store(&gTask.state, eRunning);
    if (pthread_create(&gTask.thread, NULL, taskEntry, &gTask) != 0) {
        atomic_store(&gTask.state, eDeleted);
        return pdFAIL;
    }
    gTask.threadCreated = true;
    *pTaskHandle = &gTask;
    return pdPASS;
}

TickType_t xTaskGetTickCount(void)
{
    return atomic_load(&gTickCount);
}

void vTaskDelay(TickType_t ticks)
{
    struct timespec delay = {.tv_sec = 0, .tv_nsec = 1000000};
    gLastDelay = ticks;
    atomic_fetch_add(&gTickCount, ticks);
    atomic_fetch_add(&gDelayCalls, 1);
    nanosleep(&delay, NULL);
}

void vTaskDelete(TaskHandle_t taskHandle)
{
    if (taskHandle == NULL) {
        atomic_store(&gTask.state, eDeleted);
    } else {
        atomic_store(&taskHandle->state, eDeleted);
    }
}

eTaskState eTaskGetState(TaskHandle_t taskHandle)
{
    if (taskHandle == NULL) {
        gStateCalledWithNull = true;
        return eDeleted;
    }
    return (eTaskState)atomic_load(&taskHandle->state);
}

uint32_t ulTaskNotifyTake(BaseType_t clearOnExit, TickType_t ticksToWait)
{
    (void)ticksToWait;
    pthread_mutex_lock(&gNotifyMutex);
    while (gNotificationCount == 0) {
        pthread_cond_wait(&gNotifyCondition, &gNotifyMutex);
    }
    uint32_t notificationCount = gNotificationCount;
    if (clearOnExit == pdTRUE) {
        gNotificationCount = 0;
    } else {
        gNotificationCount--;
    }
    pthread_mutex_unlock(&gNotifyMutex);
    return notificationCount;
}

BaseType_t xTaskNotifyGive(TaskHandle_t taskHandle)
{
    (void)taskHandle;
    pthread_mutex_lock(&gNotifyMutex);
    gNotificationCount++;
    pthread_cond_signal(&gNotifyCondition);
    pthread_mutex_unlock(&gNotifyMutex);
    return pdPASS;
}

void vTaskNotifyGiveFromISR(TaskHandle_t taskHandle,
                            BaseType_t *pHigherPriorityTaskWoken)
{
    xTaskNotifyGive(taskHandle);
    *pHigherPriorityTaskWoken = pdTRUE;
}

BaseType_t xSemaphoreTake(SemaphoreHandle_t semaphore, TickType_t ticks)
{
    (void)semaphore;
    gLastSemaphoreTimeout = ticks;
    return gSemaphoreResult;
}
