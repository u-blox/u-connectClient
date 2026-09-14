/*
 * Copyright 2025 u-blox
 *
 * Main application for STM32F7 Nucleo boards (NUCLEO-F767ZI) running
 * ucxclient examples.
 *
 * Clock: 216 MHz from 8 MHz HSE bypass (ST-LINK MCO). Modeled on the proven
 * STM32H7 main (ports/extra/stm32h7/src/main_stm32.c); F7 differences are the
 * clock tree (216 MHz, PWR over-drive) and the UART init (no ClockPrescaler
 * field, USARTs clock from PCLK by default).
 */

#include <stdio.h>
#include <string.h>
#include "stm32f7xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#include "u_port.h"
#include "u_cx_log.h"

/* Forward declarations */
static void SystemClock_Config(void);
static void GPIO_Init(void);
void Error_Handler(void);

/* External application main function from examples */
extern int app_main(int argc, char *argv[]);

/* FreeRTOS application task */
static void ucx_task(void *pvParameters)
{
    (void)pvParameters;

    printf("Starting application...\r\n");

    /* Run the application */
    int result = app_main(0, NULL);

    printf("app_main returned %d\r\n", result);

    /* Should not reach here */
    uPortDeinit();
    vTaskDelete(NULL);
}

int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    GPIO_Init();

    /* Send early test message */
    printf("\r\n===========================================\r\n");
    printf("NUCLEO-F7 ucxclient - Starting...\r\n");
    printf("===========================================\r\n");
    printf("SYSCLK: %lu MHz\r\n", (unsigned long)(HAL_RCC_GetSysClockFreq() / 1000000U));
    printf("Creating FreeRTOS task...\r\n");

    /* Create the main application task */
    BaseType_t result = xTaskCreate(ucx_task, "ucx", 2048, NULL, 5, NULL);

    if (result == pdPASS) {
        printf("Task created successfully\r\n");
    } else {
        printf("ERROR: Failed to create task (err=%ld)\r\n", (long)result);
        printf("FATAL: System halted\r\n");
        while (1);
    }

    printf("Starting FreeRTOS scheduler...\r\n");

    /* Start scheduler */
    vTaskStartScheduler();

    /* We should never get here as control is now taken by the scheduler */
    printf("ERROR: Scheduler returned!\r\n");
    printf("FATAL: System halted\r\n");
    while (1);
}

/**
  * @brief  System Clock Configuration for NUCLEO-F767ZI
  *         SYSCLK = 216 MHz: 8 MHz HSE bypass / M=4 * N=216 / P=2
  *         HCLK 216 MHz, APB1 54 MHz, APB2 108 MHz, PLLQ 48 MHz (USB)
  *         Requires voltage scale 1 + over-drive and 7 flash wait states.
  */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Voltage scaling for the target frequency */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;  /* ST-LINK MCO outputs 8 MHz */
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;                 /* VCO input = 8/4 = 2 MHz */
    RCC_OscInitStruct.PLL.PLLN = 216;               /* VCO output = 2 * 216 = 432 MHz */
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;     /* SYSCLK = 432/2 = 216 MHz */
    RCC_OscInitStruct.PLL.PLLQ = 9;                 /* 432/9 = 48 MHz (USB/SDMMC) */

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /* Over-drive is required to reach 216 MHz */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;   /* HCLK  = 216 MHz */
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;    /* PCLK1 =  54 MHz (max) */
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;    /* PCLK2 = 108 MHz (max) */

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
    {
        Error_Handler();
    }
}

/* UART handle for console output (printf) */
static UART_HandleTypeDef gConsoleUart;

#ifndef U_EXAMPLE_CONSOLE_RX_BUFFER_SIZE
#define U_EXAMPLE_CONSOLE_RX_BUFFER_SIZE (2048)
#endif

/* Interrupt-driven RX ring buffer for the console UART.
 *
 * uart_bridge_example.c polls exampleConsoleUartRead() from a task-context
 * loop, which can be delayed (FreeRTOS preemption, time spent blocked on
 * the module UART write) for longer than one byte period at 115200 baud
 * (~87us). The console USART's RDR is only a single byte deep, so a plain
 * blocking HAL_UART_Receive() poll silently drops bytes under sustained
 * continuous streaming (e.g. an XMODEM firmware transfer over TeraTerm) -
 * this is what made large/binary transfers get stuck. Feeding this ring
 * buffer directly from the RXNE interrupt means no byte is ever lost
 * between polls, regardless of how promptly the bridge loop drains it. */
static volatile uint8_t gConsoleRxBuffer[U_EXAMPLE_CONSOLE_RX_BUFFER_SIZE];
static volatile uint32_t gConsoleRxHead = 0;
static volatile uint32_t gConsoleRxTail = 0;

/**
 * @brief  Initialize USART3 for console output (ST-LINK VCP)
 * USART3 TX: PD8, RX: PD9 (AF7)
 * Baud: 115200, 8N1
 */
static void Console_UART_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable clocks */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_USART3_CLK_ENABLE();

    /* Configure USART3 GPIO pins (PD8 = TX, PD9 = RX) */
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    gConsoleUart.Instance = USART3;
    gConsoleUart.Init.BaudRate = 115200;
    gConsoleUart.Init.WordLength = UART_WORDLENGTH_8B;
    gConsoleUart.Init.StopBits = UART_STOPBITS_1;
    gConsoleUart.Init.Parity = UART_PARITY_NONE;
    gConsoleUart.Init.Mode = UART_MODE_TX_RX;
    gConsoleUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    gConsoleUart.Init.OverSampling = UART_OVERSAMPLING_16;
    gConsoleUart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    gConsoleUart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if (HAL_UART_Init(&gConsoleUart) != HAL_OK) {
        Error_Handler();
    }

    /* Interrupt-driven RX (see gConsoleRxBuffer comment above) so large or
     * binary transfers relayed by uart_bridge_example don't overrun the
     * single-byte RDR between polls. */
    HAL_NVIC_SetPriority(USART3_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
    __HAL_UART_ENABLE_IT(&gConsoleUart, UART_IT_RXNE);
}

/**
 * @brief  Legacy GPIO init kept for compatibility
 */
static void GPIO_Init(void)
{
    Console_UART_Init();
}

/* HAL MSP Init */
void HAL_MspInit(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    /* System interrupt init */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
}

/* HAL UART MSP Init for the u-blox module UART */
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (huart->Instance == USART6)
    {
        /* USART6 GPIO Configuration on NUCLEO-F767ZI (CN10 Arduino header):
         * PG14: USART6_TX (Arduino D1)
         * PG9:  USART6_RX (Arduino D0)
         * Verified against ST STM32F767ZITx.xml (D0=PG_9, D1=PG_14, AF8).
         */
        __HAL_RCC_GPIOG_CLK_ENABLE();
        __HAL_RCC_USART6_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_14;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
        HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

        /* Configure CTS/RTS if hardware flow control is enabled
         * (USART6 CTS = PG13, RTS = PG8, AF8) */
        if (huart->Init.HwFlowCtl == UART_HWCONTROL_RTS_CTS)
        {
            GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_13;
            GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
            HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
        }
    }
    else if (huart->Instance == USART3)
    {
        /* USART3 (console) already initialized in Console_UART_Init */
    }
}

/* FreeRTOS Hooks */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    printf("\r\nFATAL: Stack overflow in task '%s'\r\n", pcTaskName);
    printf("FATAL: System halted\r\n");
    while (1);
}

void vApplicationMallocFailedHook(void)
{
    printf("\r\nFATAL: Malloc failed - out of heap memory\r\n");
    printf("FATAL: System halted\r\n");
    while (1);
}

/* Error handler */
void Error_Handler(void)
{
    printf("\r\nFATAL: Error_Handler() called\r\n");
    printf("FATAL: System halted\r\n");
    __disable_irq();
    while (1);
}

/* HAL assert handler */
void assert_failed(uint8_t* file, uint32_t line)
{
    printf("\r\nFATAL: Assert failed at %s:%lu\r\n", (char*)file, line);
    printf("FATAL: System halted\r\n");
    while (1);
}

/* Retarget printf to the console UART via __io_putchar (used by syscalls.c _write) */
int __io_putchar(int ch)
{
    /* Use HAL_UART_Transmit with a reasonable timeout */
    uint8_t c = (uint8_t)ch;
    if (HAL_UART_Transmit(&gConsoleUart, &c, 1, 1000) == HAL_OK) {
        return ch;
    }
    return -1;
}

/**
 * @brief Console UART RX interrupt handler - called from USART3_IRQHandler
 * (see stm32f7xx_it.c). Reads the raw data register directly instead of
 * going through HAL_UART_IRQHandler()/HAL_UART_RxCpltCallback(), since that
 * callback name is already defined (for the module UART instance) by the
 * generic port driver in u_port_uart_stm32f7.c and cannot be redefined here.
 */
void exampleConsoleUart_IRQHandler(void)
{
    if (__HAL_UART_GET_FLAG(&gConsoleUart, UART_FLAG_RXNE)) {
        uint8_t byte = (uint8_t)gConsoleUart.Instance->RDR;
        uint32_t nextHead = (gConsoleRxHead + 1) % U_EXAMPLE_CONSOLE_RX_BUFFER_SIZE;
        if (nextHead != gConsoleRxTail) {
            gConsoleRxBuffer[gConsoleRxHead] = byte;
            gConsoleRxHead = nextHead;
        }
        /* else: buffer full, drop the byte (reader too slow) */
    }
    if (__HAL_UART_GET_FLAG(&gConsoleUart, UART_FLAG_ORE)) {
        /* Overrun can only happen if RXNE wasn't serviced for >1 byte time;
         * clear it so reception keeps going. */
        __HAL_UART_CLEAR_OREFLAG(&gConsoleUart);
    }
}

/*
 * Raw console UART access for uart_bridge_example. Bypasses the printf
 * retarget above so the example can both read and write the ST-Link VCP
 * UART directly (needed to bridge it with the u-blox module UART).
 *
 * exampleConsoleUartRead() pops from the interrupt-fed ring buffer above
 * instead of calling HAL_UART_Receive() directly - see gConsoleRxBuffer
 * comment for why.
 */
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
