#ifndef STM32F4XX_HAL_H
#define STM32F4XX_HAL_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t identifier;
} USART_TypeDef;

extern USART_TypeDef gFakeUsart2;
extern USART_TypeDef gFakeUsart3;

#define USART2 (&gFakeUsart2)
#define USART3 (&gFakeUsart3)
#define USART3_IRQn 39

typedef int32_t IRQn_Type;

typedef enum {
    HAL_OK = 0,
    HAL_ERROR = 1,
    HAL_BUSY = 2,
    HAL_TIMEOUT = 3
} HAL_StatusTypeDef;

typedef struct {
    uint32_t BaudRate;
    uint32_t WordLength;
    uint32_t StopBits;
    uint32_t Parity;
    uint32_t Mode;
    uint32_t HwFlowCtl;
    uint32_t OverSampling;
} UART_InitTypeDef;

typedef struct {
    USART_TypeDef *Instance;
    UART_InitTypeDef Init;
} UART_HandleTypeDef;

#define UART_WORDLENGTH_8B 8U
#define UART_STOPBITS_1 1U
#define UART_PARITY_NONE 0U
#define UART_MODE_TX_RX 3U
#define UART_HWCONTROL_NONE 0U
#define UART_HWCONTROL_RTS_CTS 3U
#define UART_OVERSAMPLING_16 16U
#define HAL_MAX_DELAY UINT32_MAX

typedef struct {
    HAL_StatusTypeDef initStatus;
    HAL_StatusTypeDef deinitStatus;
    HAL_StatusTypeDef transmitStatus;
    HAL_StatusTypeDef receiveStatus;
    UART_HandleTypeDef *pLastUart;
    UART_HandleTypeDef *pRxUart;
    uint8_t *pRxByte;
    const uint8_t *pTxData;
    uint16_t txLength;
    uint32_t txTimeout;
    uint32_t tick;
    uint32_t tickIncrement;
    uint32_t initCalls;
    uint32_t deinitCalls;
    uint32_t transmitCalls;
    uint32_t receiveCalls;
    uint32_t irqHandlerCalls;
    uint32_t nvicSetPriorityCalls;
    uint32_t nvicEnableCalls;
    uint32_t nvicDisableCalls;
    uint32_t clockEnableCalls;
    uint32_t clockDisableCalls;
    IRQn_Type lastIrq;
    uint32_t lastPriority;
    uint32_t lastSubPriority;
} fakeHalState_t;

extern fakeHalState_t gFakeHal;

void fakeHalReset(void);
void fakeHalInjectRxByte(uint8_t value);

HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *pUart);
HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *pUart);
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *pUart,
                                   uint8_t *pData,
                                   uint16_t length,
                                   uint32_t timeout);
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *pUart,
                                     uint8_t *pData,
                                     uint16_t length);
void HAL_UART_IRQHandler(UART_HandleTypeDef *pUart);
uint32_t HAL_GetTick(void);
void HAL_NVIC_SetPriority(IRQn_Type irq, uint32_t priority,
                          uint32_t subPriority);
void HAL_NVIC_EnableIRQ(IRQn_Type irq);
void HAL_NVIC_DisableIRQ(IRQn_Type irq);
void fakeHalUartClockEnable(void);
void fakeHalUartClockDisable(void);

#define __HAL_RCC_USART3_CLK_ENABLE() fakeHalUartClockEnable()
#define __HAL_RCC_USART3_CLK_DISABLE() fakeHalUartClockDisable()

#endif
