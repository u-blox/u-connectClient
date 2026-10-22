/*
 * Copyright 2025 u-blox
 *
 * Main application for the NUCLEO-G071RB running u-connectClient examples.
 *
 * Console: USART2 (PA2 TX, PA3 RX) = ST-LINK virtual COM port, 115200 8N1.
 * Module:  USART1 (PA9/D8 TX, PA10/D2 RX, PA11 CTS, PA12 RTS), see u_port_uart_stm32f4.h.
 */

#include <stdio.h>
#include <string.h>
#include "stm32g0xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#include "u_port.h"
#include "u_cx_log.h"

static void SystemClock_Config(void);
static void Console_UART_Init(void);
void Error_Handler(void);

extern int app_main(int argc, char *argv[]);

static void ucx_task(void *pvParameters)
{
    (void)pvParameters;

    int result = app_main(0, NULL);

    printf("app_main returned %d\r\n", result);
    uPortDeinit();
    vTaskDelete(NULL);
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    Console_UART_Init();

    printf("\r\nNUCLEO-G071RB u-connectClient - Starting...\r\n");

    /* Small start task: app_main() creates the real application task and deletes this one. */
    if (xTaskCreate(ucx_task, "ucx", 256, NULL, 5, NULL) != pdPASS) {
        printf("FATAL: cannot create task\r\n");
        while (1);
    }
    vTaskStartScheduler();

    printf("FATAL: Scheduler returned!\r\n");
    while (1);
}

/* HSI16, PLL M=1 N=8 R=2 = 64 MHz SYSCLK and PCLK, 2 flash wait states. */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
    RCC_OscInitStruct.PLL.PLLN = 8;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

static UART_HandleTypeDef gConsoleUart;

#ifndef U_EXAMPLE_CONSOLE_RX_BUFFER_SIZE
#define U_EXAMPLE_CONSOLE_RX_BUFFER_SIZE (256)
#endif

/* Interrupt-fed console RX ring so bytes are not lost between polls. */
static volatile uint8_t gConsoleRxBuffer[U_EXAMPLE_CONSOLE_RX_BUFFER_SIZE];
static volatile uint32_t gConsoleRxHead = 0;
static volatile uint32_t gConsoleRxTail = 0;

static void Console_UART_Init(void)
{
    gConsoleUart.Instance = USART2;
    gConsoleUart.Init.BaudRate = 115200;
    gConsoleUart.Init.WordLength = UART_WORDLENGTH_8B;
    gConsoleUart.Init.StopBits = UART_STOPBITS_1;
    gConsoleUart.Init.Parity = UART_PARITY_NONE;
    gConsoleUart.Init.Mode = UART_MODE_TX_RX;
    gConsoleUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    gConsoleUart.Init.OverSampling = UART_OVERSAMPLING_16;
    gConsoleUart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    if (HAL_UART_Init(&gConsoleUart) != HAL_OK) {
        Error_Handler();
    }
    HAL_NVIC_SetPriority(USART2_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    __HAL_UART_ENABLE_IT(&gConsoleUart, UART_IT_RXNE);
}

void HAL_MspInit(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    if (huart->Instance == USART2) {
        __HAL_RCC_USART2_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    } else if (huart->Instance == USART1) {
        __HAL_RCC_USART1_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
        if (huart->Init.HwFlowCtl == UART_HWCONTROL_RTS_CTS) {
            GPIO_InitStruct.Pin |= GPIO_PIN_11 | GPIO_PIN_12;
        }
        GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    printf("\r\nFATAL: Stack overflow in task '%s'\r\n", pcTaskName);
    while (1);
}

void vApplicationMallocFailedHook(void)
{
    printf("\r\nFATAL: Malloc failed - out of heap memory\r\n");
    while (1);
}

void Error_Handler(void)
{
    printf("\r\nFATAL: Error_Handler() called\r\n");
    __disable_irq();
    while (1);
}

void assert_failed(uint8_t *file, uint32_t line)
{
    printf("\r\nFATAL: Assert failed at %s:%lu\r\n", (char *)file, line);
    while (1);
}

int __io_putchar(int ch)
{
    uint8_t c = (uint8_t)ch;
    if (HAL_UART_Transmit(&gConsoleUart, &c, 1, 1000) == HAL_OK) {
        return ch;
    }
    return -1;
}

/* Called from USART2_IRQHandler (stm32g0xx_it.c). */
void exampleConsoleUart_IRQHandler(void)
{
    if (__HAL_UART_GET_FLAG(&gConsoleUart, UART_FLAG_RXNE)) {
        uint8_t byte = (uint8_t)gConsoleUart.Instance->RDR;
        uint32_t nextHead = (gConsoleRxHead + 1) % U_EXAMPLE_CONSOLE_RX_BUFFER_SIZE;
        if (nextHead != gConsoleRxTail) {
            gConsoleRxBuffer[gConsoleRxHead] = byte;
            gConsoleRxHead = nextHead;
        }
    }
    if (__HAL_UART_GET_FLAG(&gConsoleUart, UART_FLAG_ORE)) {
        __HAL_UART_CLEAR_OREFLAG(&gConsoleUart);
    }
}

int32_t exampleConsoleUartRead(uint8_t *pByte, int32_t timeoutMs)
{
    uint32_t startTime = HAL_GetTick();

    do {
        uint32_t tail = gConsoleRxTail;
        if (gConsoleRxHead != tail) {
            *pByte = gConsoleRxBuffer[tail];
            gConsoleRxTail = (tail + 1) % U_EXAMPLE_CONSOLE_RX_BUFFER_SIZE;
            return 1;
        }
    } while ((timeoutMs > 0) && ((int32_t)(HAL_GetTick() - startTime) < timeoutMs));

    return 0;
}

int32_t exampleConsoleUartWrite(const void *pData, size_t length)
{
    if (HAL_UART_Transmit(&gConsoleUart, (uint8_t *)pData, (uint16_t)length, 1000) == HAL_OK) {
        return (int32_t)length;
    }
    return -1;
}
