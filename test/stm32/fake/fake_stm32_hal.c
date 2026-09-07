#include <string.h>

#include "stm32f4xx_hal.h"

USART_TypeDef gFakeUsart2 = {.identifier = 2};
USART_TypeDef gFakeUsart3 = {.identifier = 3};
fakeHalState_t gFakeHal;

extern void HAL_UART_RxCpltCallback(UART_HandleTypeDef *pUart);

void fakeHalReset(void)
{
    memset(&gFakeHal, 0, sizeof(gFakeHal));
    gFakeHal.initStatus = HAL_OK;
    gFakeHal.deinitStatus = HAL_OK;
    gFakeHal.transmitStatus = HAL_OK;
    gFakeHal.receiveStatus = HAL_OK;
}

void fakeHalInjectRxByte(uint8_t value)
{
    if ((gFakeHal.pRxUart != NULL) && (gFakeHal.pRxByte != NULL)) {
        *gFakeHal.pRxByte = value;
        HAL_UART_RxCpltCallback(gFakeHal.pRxUart);
    }
}

HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *pUart)
{
    gFakeHal.initCalls++;
    gFakeHal.pLastUart = pUart;
    return gFakeHal.initStatus;
}

HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *pUart)
{
    gFakeHal.deinitCalls++;
    gFakeHal.pLastUart = pUart;
    return gFakeHal.deinitStatus;
}

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *pUart,
                                   uint8_t *pData,
                                   uint16_t length,
                                   uint32_t timeout)
{
    gFakeHal.transmitCalls++;
    gFakeHal.pLastUart = pUart;
    gFakeHal.pTxData = pData;
    gFakeHal.txLength = length;
    gFakeHal.txTimeout = timeout;
    return gFakeHal.transmitStatus;
}

HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *pUart,
                                     uint8_t *pData,
                                     uint16_t length)
{
    gFakeHal.receiveCalls++;
    gFakeHal.pRxUart = pUart;
    gFakeHal.pRxByte = pData;
    return length == 1 ? gFakeHal.receiveStatus : HAL_ERROR;
}

void HAL_UART_IRQHandler(UART_HandleTypeDef *pUart)
{
    gFakeHal.irqHandlerCalls++;
    gFakeHal.pLastUart = pUart;
}

uint32_t HAL_GetTick(void)
{
    uint32_t tick = gFakeHal.tick;
    gFakeHal.tick += gFakeHal.tickIncrement;
    return tick;
}

void HAL_NVIC_SetPriority(IRQn_Type irq, uint32_t priority,
                          uint32_t subPriority)
{
    gFakeHal.nvicSetPriorityCalls++;
    gFakeHal.lastIrq = irq;
    gFakeHal.lastPriority = priority;
    gFakeHal.lastSubPriority = subPriority;
}

void HAL_NVIC_EnableIRQ(IRQn_Type irq)
{
    gFakeHal.nvicEnableCalls++;
    gFakeHal.lastIrq = irq;
}

void HAL_NVIC_DisableIRQ(IRQn_Type irq)
{
    gFakeHal.nvicDisableCalls++;
    gFakeHal.lastIrq = irq;
}

void fakeHalUartClockEnable(void)
{
    gFakeHal.clockEnableCalls++;
}

void fakeHalUartClockDisable(void)
{
    gFakeHal.clockDisableCalls++;
}
