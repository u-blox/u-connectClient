/* FreeRTOS configuration for the NUCLEO-G071RB (Cortex-M0+, 36 KB RAM). */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
 #include <stdint.h>
 extern uint32_t SystemCoreClock;
#endif

#define configUSE_PREEMPTION                    1
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configCPU_CLOCK_HZ                      ( SystemCoreClock )
#define configTICK_RATE_HZ                      ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES                    ( 8 )
#define configMINIMAL_STACK_SIZE                ( ( uint16_t ) 96 )
#ifndef configTOTAL_HEAP_SIZE
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 12 * 1024 ) )
#endif
#define configMAX_TASK_NAME_LEN                 ( 8 )
#define configUSE_TRACE_FACILITY                0
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_MUTEXES                       1
#define configQUEUE_REGISTRY_SIZE               0
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_MALLOC_FAILED_HOOK            1
#define configUSE_APPLICATION_TASK_TAG          0
#define configUSE_COUNTING_SEMAPHORES           1
#define configGENERATE_RUN_TIME_STATS           0

#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         ( 2 )

#define configUSE_TIMERS                        0
#define configTIMER_TASK_PRIORITY               ( 2 )
#define configTIMER_QUEUE_LENGTH                4
#define configTIMER_TASK_STACK_DEPTH            ( configMINIMAL_STACK_SIZE * 2 )

#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           0
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 0
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1

/* Cortex-M0+ implements 2 priority bits; the CM0 port masks interrupts with PRIMASK. */
#ifdef __NVIC_PRIO_BITS
 #define configPRIO_BITS                        __NVIC_PRIO_BITS
#else
 #define configPRIO_BITS                        2
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         3
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    1

#define configKERNEL_INTERRUPT_PRIORITY         ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }

#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
/* SysTick_Handler is defined in stm32g0xx_it.c */

#endif /* FREERTOS_CONFIG_H */
