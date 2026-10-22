# NUCLEO-64 boards with an STM32F4 (NUCLEO-F401RE, NUCLEO-F446RE)
# Set STM32_CHIP, STM32_STARTUP_NAME, STM32_LINKER_NAME and STM32_BOARD_DEFINE before including.
set(STM32_FAMILY "STM32F4xx")

# CPU specific flags - Cortex-M4F: hard-float FPU (fpv4-sp-d16)
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")

if(NOT DEFINED STM32_HAL_PATH)
    set(STM32_HAL_PATH "${CMAKE_CURRENT_LIST_DIR}/../ports/extra/stm32f4/STM32CubeF4" CACHE PATH "Path to STM32CubeF4")
endif()

if(NOT DEFINED FREERTOS_PATH)
    set(FREERTOS_PATH "${STM32_HAL_PATH}/Middlewares/Third_Party/FreeRTOS/Source" CACHE PATH "Path to FreeRTOS Source")
endif()

set(STM32_HAL_INCLUDE_DIRS
    ${STM32_HAL_PATH}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${STM32_HAL_PATH}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${STM32_HAL_PATH}/Drivers/CMSIS/Include
)

set(STM32_HAL_SOURCES
    ${STM32_HAL_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
    ${STM32_HAL_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
    ${STM32_HAL_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
    ${STM32_HAL_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
    ${STM32_HAL_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
    ${STM32_HAL_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
    ${STM32_HAL_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
    ${STM32_HAL_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
)

set(FREERTOS_INCLUDE_DIRS
    ${FREERTOS_PATH}/include
    ${FREERTOS_PATH}/portable/GCC/ARM_CM4F
    ${FREERTOS_PATH}/CMSIS_RTOS
)

set(FREERTOS_SOURCES
    ${FREERTOS_PATH}/tasks.c
    ${FREERTOS_PATH}/queue.c
    ${FREERTOS_PATH}/list.c
    ${FREERTOS_PATH}/timers.c
    ${FREERTOS_PATH}/portable/GCC/ARM_CM4F/port.c
    ${FREERTOS_PATH}/portable/MemMang/heap_4.c
    ${FREERTOS_PATH}/CMSIS_RTOS/cmsis_os.c
)

set(STM32_STARTUP_FILE "${STM32_HAL_PATH}/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/${STM32_STARTUP_NAME}")
set(STM32_LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/../ports/extra/stm32f4/scripts/${STM32_LINKER_NAME}")

# The F401 has no TIM6, so the HAL tick stays on SysTick (no stm32f4xx_hal_timebase_tim.c).
set(STM32_NO_TIM_TIMEBASE ON)

add_compile_definitions(
    ${STM32_CHIP}
    ${STM32_BOARD_DEFINE}
    NUCLEO64
    USE_HAL_DRIVER
    HSE_VALUE=8000000
)
