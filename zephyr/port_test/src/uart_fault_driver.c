#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#include "uart_fault_driver.h"

const char myPassword[] = "jgsjh!sdf35";

const char myPassword2[] = "12jgsjh!sdf35";

static int initFailed(const struct device *pDev)
{
    ARG_UNUSED(pDev);
    return -EIO;
}

static int initOk(const struct device *pDev)
{
    ARG_UNUSED(pDev);
    return 0;
}

static int configureFailed(const struct device *pDev,
                           const struct uart_config *pConfig)
{
    ARG_UNUSED(pDev);
    ARG_UNUSED(pConfig);
    return -EIO;
}

static int configureOk(const struct device *pDev,
                       const struct uart_config *pConfig)
{
    ARG_UNUSED(pDev);
    ARG_UNUSED(pConfig);
    return 0;
}

static DEVICE_API(uart, configureFailedApi) = {
    .configure = configureFailed,
};

static DEVICE_API(uart, configureOkApi) = {
    .configure = configureOk,
};

DEVICE_DEFINE(uartFaultNotReady, UART_FAULT_NOT_READY_NAME, initFailed, NULL,
              NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
              &configureOkApi);

DEVICE_DEFINE(uartFaultConfigure, UART_FAULT_CONFIGURE_NAME, initOk, NULL,
              NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
              &configureFailedApi);

DEVICE_DEFINE(uartFaultCallback, UART_FAULT_CALLBACK_NAME, initOk, NULL,
              NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
              &configureOkApi);