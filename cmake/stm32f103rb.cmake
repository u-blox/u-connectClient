# NUCLEO-F103RB specific settings
set(STM32_CHIP "STM32F103xB")
set(STM32_FAMILY "STM32F1xx")
set(STM32_FAMILY_SHORT "F1")

# Cortex-M3, no FPU
set(CPU_FLAGS "-mcpu=cortex-m3 -mthumb -mfloat-abi=soft")

set(CMAKE_C_FLAGS_INIT "${CPU_FLAGS} -fdata-sections -ffunction-sections -Wall")
set(CMAKE_CXX_FLAGS_INIT "${CPU_FLAGS} -fdata-sections -ffunction-sections -Wall")
set(CMAKE_ASM_FLAGS_INIT "${CPU_FLAGS} -x assembler-with-cpp")

set(CMAKE_EXE_LINKER_FLAGS_INIT "${CPU_FLAGS} -specs=nano.specs -specs=nosys.specs -Wl,--gc-sections")

if(NOT DEFINED STM32_HAL_PATH)
    set(STM32_HAL_PATH "${CMAKE_CURRENT_LIST_DIR}/../ports/extra/stm32f1/STM32CubeF1" CACHE PATH "Path to STM32CubeF1")
endif()

if(NOT DEFINED FREERTOS_PATH)
    set(FREERTOS_PATH "${STM32_HAL_PATH}/Middlewares/Third_Party/FreeRTOS/Source" CACHE PATH "Path to FreeRTOS Source")
endif()

set(STM32_HAL_INCLUDE_DIRS
    ${STM32_HAL_PATH}/Drivers/STM32F1xx_HAL_Driver/Inc
    ${STM32_HAL_PATH}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    ${STM32_HAL_PATH}/Drivers/CMSIS/Include
)

set(STM32_HAL_SOURCES
    ${STM32_HAL_PATH}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
    ${STM32_HAL_PATH}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
    ${STM32_HAL_PATH}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
    ${STM32_HAL_PATH}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
    ${STM32_HAL_PATH}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
    ${STM32_HAL_PATH}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c
    ${STM32_HAL_PATH}/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
)

set(FREERTOS_INCLUDE_DIRS
    ${FREERTOS_PATH}/include
    ${FREERTOS_PATH}/portable/GCC/ARM_CM3
    ${FREERTOS_PATH}/CMSIS_RTOS
)

set(FREERTOS_SOURCES
    ${FREERTOS_PATH}/tasks.c
    ${FREERTOS_PATH}/queue.c
    ${FREERTOS_PATH}/list.c
    ${FREERTOS_PATH}/timers.c
    ${FREERTOS_PATH}/portable/GCC/ARM_CM3/port.c
    ${FREERTOS_PATH}/portable/MemMang/heap_4.c
    ${FREERTOS_PATH}/CMSIS_RTOS/cmsis_os.c
)

set(STM32_STARTUP_FILE "${STM32_HAL_PATH}/Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103xb.s")

set(STM32_LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/../ports/extra/stm32f1/scripts/STM32F103RBTX_FLASH.ld")

add_compile_definitions(
    ${STM32_CHIP}
    USE_HAL_DRIVER
    HSE_VALUE=8000000
    NUCLEO_F103RB
)
