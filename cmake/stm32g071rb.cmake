# NUCLEO-G071RB specific settings
set(STM32_CHIP "STM32G071xx")
set(STM32_FAMILY "STM32G0xx")
set(STM32_FAMILY_SHORT "G0")

# Cortex-M0+, no FPU
set(CPU_FLAGS "-mcpu=cortex-m0plus -mthumb -mfloat-abi=soft")

set(CMAKE_C_FLAGS_INIT "${CPU_FLAGS} -fdata-sections -ffunction-sections -Wall")
set(CMAKE_CXX_FLAGS_INIT "${CPU_FLAGS} -fdata-sections -ffunction-sections -Wall")
set(CMAKE_ASM_FLAGS_INIT "${CPU_FLAGS} -x assembler-with-cpp")

set(CMAKE_EXE_LINKER_FLAGS_INIT "${CPU_FLAGS} -specs=nano.specs -specs=nosys.specs -Wl,--gc-sections")

if(NOT DEFINED STM32_HAL_PATH)
    set(STM32_HAL_PATH "${CMAKE_CURRENT_LIST_DIR}/../ports/extra/stm32g0/STM32CubeG0" CACHE PATH "Path to STM32CubeG0")
endif()

if(NOT DEFINED FREERTOS_PATH)
    set(FREERTOS_PATH "${STM32_HAL_PATH}/Middlewares/Third_Party/FreeRTOS/Source" CACHE PATH "Path to FreeRTOS Source")
endif()

set(STM32_HAL_INCLUDE_DIRS
    ${STM32_HAL_PATH}/Drivers/STM32G0xx_HAL_Driver/Inc
    ${STM32_HAL_PATH}/Drivers/CMSIS/Device/ST/STM32G0xx/Include
    ${STM32_HAL_PATH}/Drivers/CMSIS/Include
)

set(_G0_HAL ${STM32_HAL_PATH}/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal)
set(STM32_HAL_SOURCES
    ${_G0_HAL}.c
    ${_G0_HAL}_cortex.c
    ${_G0_HAL}_rcc.c
    ${_G0_HAL}_rcc_ex.c
    ${_G0_HAL}_gpio.c
    ${_G0_HAL}_uart.c
    ${_G0_HAL}_uart_ex.c
    ${_G0_HAL}_dma.c
    ${_G0_HAL}_dma_ex.c
    ${_G0_HAL}_pwr.c
    ${_G0_HAL}_pwr_ex.c
)

set(FREERTOS_INCLUDE_DIRS
    ${FREERTOS_PATH}/include
    ${FREERTOS_PATH}/portable/GCC/ARM_CM0
    ${FREERTOS_PATH}/CMSIS_RTOS
)

set(FREERTOS_SOURCES
    ${FREERTOS_PATH}/tasks.c
    ${FREERTOS_PATH}/queue.c
    ${FREERTOS_PATH}/list.c
    ${FREERTOS_PATH}/timers.c
    ${FREERTOS_PATH}/portable/GCC/ARM_CM0/port.c
    ${FREERTOS_PATH}/portable/MemMang/heap_4.c
    ${FREERTOS_PATH}/CMSIS_RTOS/cmsis_os.c
)

set(STM32_STARTUP_FILE "${STM32_HAL_PATH}/Drivers/CMSIS/Device/ST/STM32G0xx/Source/Templates/gcc/startup_stm32g071xx.s")

set(STM32_LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/../ports/extra/stm32g0/scripts/STM32G071RBTX_FLASH.ld")

add_compile_definitions(
    ${STM32_CHIP}
    USE_HAL_DRIVER
    G071RB
)
