cmake_minimum_required(VERSION 3.16)
project(u_connect_client_stm32_tests C)

include(CTest)
find_package(Threads REQUIRED)

option(ENABLE_COVERAGE "Build with gcov instrumentation" ON)
option(ENABLE_SANITIZERS "Build with address and undefined sanitizers" ON)

add_executable(test_stm32_uart
    test_stm32_uart.c
    fake/fake_stm32_hal.c
    ../../ports/uart/u_port_uart_stm32f4.c
)

target_include_directories(test_stm32_uart PRIVATE
    fake
    ../../ports
    ../../ports/uart
)
target_compile_options(test_stm32_uart PRIVATE
    -Wall
    -Wextra
    -Werror
    -pedantic
)
target_link_libraries(test_stm32_uart PRIVATE Threads::Threads)

if(ENABLE_COVERAGE)
    target_compile_options(test_stm32_uart PRIVATE --coverage -O0 -g)
    target_link_options(test_stm32_uart PRIVATE --coverage)
endif()

if(ENABLE_SANITIZERS)
    target_compile_options(test_stm32_uart PRIVATE
        -fsanitize=address
        -fsanitize=undefined
        -fno-sanitize-recover
        -fno-omit-frame-pointer
    )
    target_link_options(test_stm32_uart PRIVATE
        -fsanitize=address
        -fsanitize=undefined
        -fno-sanitize-recover
    )
endif()

add_test(NAME stm32_uart COMMAND test_stm32_uart)
set_tests_properties(stm32_uart PROPERTIES TIMEOUT 10)

add_executable(test_freertos_port
    test_freertos_port.c
    fake/fake_freertos.c
    ../../ports/os/u_port_freertos.c
    ../../src/u_cx_log.c
)
target_compile_definitions(test_freertos_port PRIVATE U_PORT_FREERTOS)
target_include_directories(test_freertos_port PRIVATE
    fake
    ../../inc
    ../../ports
)
target_compile_options(test_freertos_port PRIVATE
    -Wall
    -Wextra
    -Werror
    -pedantic
)
target_link_libraries(test_freertos_port PRIVATE Threads::Threads)
if(ENABLE_COVERAGE)
    target_compile_options(test_freertos_port PRIVATE --coverage -O0 -g)
    target_link_options(test_freertos_port PRIVATE --coverage)
endif()
if(ENABLE_SANITIZERS)
    target_compile_options(test_freertos_port PRIVATE
        -fsanitize=address
        -fsanitize=undefined
        -fno-sanitize-recover
        -fno-omit-frame-pointer
    )
    target_link_options(test_freertos_port PRIVATE
        -fsanitize=address
        -fsanitize=undefined
        -fno-sanitize-recover
    )
endif()

add_test(NAME freertos_port COMMAND test_freertos_port)
set_tests_properties(freertos_port PROPERTIES TIMEOUT 10)
