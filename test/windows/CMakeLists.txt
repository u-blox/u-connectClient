cmake_minimum_required(VERSION 3.16)
project(u_connect_client_windows_tests C)

include(CTest)

function(configure_windows_test target)
    target_compile_definitions(${target} PRIVATE
        U_PORT_WINDOWS
        U_CX_LOG_AT=0
        U_CX_LOG_DEBUG=0
        U_CX_LOG_WARNING=0
        U_CX_LOG_ERROR=0
        U_CX_LOG_PRINT_TIME=0
    )
    target_include_directories(${target} PRIVATE
        fake
        ../../inc
        ../../ports
    )
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Werror
            -pedantic
        )
    endif()
endfunction()

add_executable(test_windows_uart
    test_windows_uart.c
    fake/fake_windows.c
    ../../ports/uart/u_port_uart_windows.c
    ../../src/u_cx_log.c
)
configure_windows_test(test_windows_uart)
add_test(NAME windows_uart COMMAND test_windows_uart)

add_executable(test_windows_port
    test_windows_port.c
    fake/fake_windows.c
    ../../ports/os/u_port_windows.c
    ../../src/u_cx_log.c
)
configure_windows_test(test_windows_port)
add_test(NAME windows_port COMMAND test_windows_port)