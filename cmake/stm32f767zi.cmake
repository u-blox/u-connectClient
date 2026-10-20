# NUCLEO-F767ZI specific settings
# STM32F767ZI: Cortex-M7 @ 216 MHz, 512 KB RAM, 2 MB Flash
# Modeled on stm32h753zi.cmake so the F7 build matches the proven H7 one.
set(STM32_CHIP "STM32F767xx")
set(STM32_FAMILY "STM32F7xx")
set(STM32_FAMILY_SHORT "F7")

# CPU specific flags - Cortex-M7 with double-precision FPU (hard float ABI)
set(CPU_FLAGS "-mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-d16")

# Compiler flags
set(CMAKE_C_FLAGS_INIT "${CPU_FLAGS} -fdata-sections -ffunction-sections -Wall")
set(CMAKE_CXX_FLAGS_INIT "${CPU_FLAGS} -fdata-sections -ffunction-sections -Wall")
set(CMAKE_ASM_FLAGS_INIT "${CPU_FLAGS} -x assembler-with-cpp")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "${CPU_FLAGS} -specs=nano.specs -specs=nosys.specs -Wl,--gc-sections")

# STM32 HAL path (submodule, can be overridden with -DSTM32_HAL_PATH)
if(NOT DEFINED STM32_HAL_PATH)
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../ports/extra/stm32f7/STM32CubeF7/Drivers")
        set(STM32_HAL_PATH "${CMAKE_CURRENT_LIST_DIR}/../ports/extra/stm32f7/STM32CubeF7" CACHE PATH "Path to STM32CubeF7")
    elseif(DEFINED ENV{STM32CUBEF7_PATH})
        set(STM32_HAL_PATH "$ENV{STM32CUBEF7_PATH}" CACHE PATH "Path to STM32CubeF7")
    else()
        message(FATAL_ERROR "STM32CubeF7 not found. Init the ports/extra/stm32f7/STM32CubeF7 submodule or set STM32_HAL_PATH / STM32CUBEF7_PATH.")
    endif()
endif()

# FreeRTOS path (from STM32CubeF7 middleware)
if(NOT DEFINED FREERTOS_PATH)
    set(FREERTOS_PATH "${STM32_HAL_PATH}/Middlewares/Third_Party/FreeRTOS/Source" CACHE PATH "Path to FreeRTOS Source")
endif()

# STM32 HAL includes
set(STM32_HAL_INCLUDE_DIRS
    ${STM32_HAL_PATH}/Drivers/STM32F7xx_HAL_Driver/Inc
    ${STM32_HAL_PATH}/Drivers/CMSIS/Device/ST/STM32F7xx/Include
    ${STM32_HAL_PATH}/Drivers/CMSIS/Include
)

# STM32 HAL sources (add only what's needed)
set(STM32_HAL_SOURCES
    ${STM32_HAL_PATH}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c
    ${STM32_HAL_PATH}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c
    ${STM32_HAL_PATH}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c
    ${STM32_HAL_PATH}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c
    ${STM32_HAL_PATH}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c
    ${STM32_HAL_PATH}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.c
    ${STM32_HAL_PATH}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart_ex.c
    ${STM32_HAL_PATH}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c
    ${STM32_HAL_PATH}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma_ex.c
    ${STM32_HAL_PATH}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.c
    ${STM32_HAL_PATH}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c
    ${STM32_HAL_PATH}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.c
    ${STM32_HAL_PATH}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim_ex.c
)

# FreeRTOS includes - Cortex-M7 r0p1 port (hard float)
set(FREERTOS_INCLUDE_DIRS
    ${FREERTOS_PATH}/include
    ${FREERTOS_PATH}/portable/GCC/ARM_CM7/r0p1
    ${FREERTOS_PATH}/CMSIS_RTOS
)

# FreeRTOS sources - Cortex-M7 r0p1 port
set(FREERTOS_SOURCES
    ${FREERTOS_PATH}/tasks.c
    ${FREERTOS_PATH}/queue.c
    ${FREERTOS_PATH}/list.c
    ${FREERTOS_PATH}/timers.c
    ${FREERTOS_PATH}/portable/GCC/ARM_CM7/r0p1/port.c
    ${FREERTOS_PATH}/portable/MemMang/heap_4.c
    ${FREERTOS_PATH}/CMSIS_RTOS/cmsis_os.c
)

# Startup file (GCC template copied into src/)
set(STM32_STARTUP_FILE "${CMAKE_CURRENT_LIST_DIR}/../ports/extra/stm32f7/src/startup_stm32f767xx.s")

# Linker script - STM32F767ZITx (2 MB flash; .bss/heap in SRAM = DMA-accessible)
set(STM32_LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/../ports/extra/stm32f7/scripts/STM32F767ZITx_FLASH.ld")

# Compile definitions
# NUCLEO-F767ZI: 8 MHz HSE from ST-LINK MCO (bypass mode)
add_compile_definitions(
    ${STM32_CHIP}
    NUCLEO_F767ZI
    USE_HAL_DRIVER
    HSE_VALUE=8000000
)
