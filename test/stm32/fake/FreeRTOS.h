#ifndef FREERTOS_H
#define FREERTOS_H

#include <stdint.h>

typedef int BaseType_t;
typedef uint32_t TickType_t;

#define pdTRUE 1
#define pdFALSE 0
#define pdPASS 1
#define pdFAIL 0
#define portMAX_DELAY UINT32_MAX
#define portTICK_PERIOD_MS 1U
#define configMAX_PRIORITIES 10U
#define pdMS_TO_TICKS(milliseconds) ((TickType_t)(milliseconds))
#define portYIELD_FROM_ISR(taskWoken) ((void)(taskWoken))

#endif
