/* HAL configuration for the NUCLEO-F103RB: only the modules the port uses. */

#ifndef __STM32F1xx_HAL_CONF_H
#define __STM32F1xx_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED

#if !defined(HSE_VALUE)
#define HSE_VALUE               8000000U
#endif
#if !defined(HSE_STARTUP_TIMEOUT)
#define HSE_STARTUP_TIMEOUT     100U
#endif
#if !defined(HSI_VALUE)
#define HSI_VALUE               8000000U
#endif
#if !defined(LSI_VALUE)
#define LSI_VALUE               40000U
#endif
#if !defined(LSE_VALUE)
#define LSE_VALUE               32768U
#endif
#if !defined(LSE_STARTUP_TIMEOUT)
#define LSE_STARTUP_TIMEOUT     5000U
#endif

#define VDD_VALUE               3300U
#define TICK_INT_PRIORITY       0x0FU
#define USE_RTOS                0U
#define PREFETCH_ENABLE         1U

#define USE_HAL_UART_REGISTER_CALLBACKS 0U

#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_cortex.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_pwr.h"
#include "stm32f1xx_hal_uart.h"

#define assert_param(expr) ((void)0U)

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_HAL_CONF_H */
