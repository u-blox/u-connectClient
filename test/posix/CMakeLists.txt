cmake_minimum_required(VERSION 3.16)
project(u_connect_client_posix_tests C)

include(CTest)
find_package(Threads REQUIRED)

option(ENABLE_COVERAGE "Build with gcov instrumentation" ON)
option(ENABLE_SANITIZERS "Build with address and undefined sanitizers" ON)

add_executable(test_posix_port
    test_posix_port.c
    ../../ports/os/u_port_posix.c
    ../../ports/uart/u_port_uart_linux.c
    ../../src/u_cx_log.c
)

target_compile_definitions(test_posix_port PRIVATE U_PORT_POSIX)
target_include_directories(test_posix_port PRIVATE
    ../../inc
    ../../ports
)
target_compile_options(test_posix_port PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wconversion
    -Wsign-conversion
    -Wshadow
    -pedantic
)
target_link_libraries(test_posix_port PRIVATE Threads::Threads util)
target_link_options(test_posix_port PRIVATE
    -Wl,--wrap=pthread_create
    -Wl,--wrap=pthread_join
)

if(ENABLE_COVERAGE)
    target_compile_options(test_posix_port PRIVATE --coverage -O0 -g)
    target_link_options(test_posix_port PRIVATE --coverage)
endif()

if(ENABLE_SANITIZERS)
    target_compile_options(test_posix_port PRIVATE
        -fsanitize=address
        -fsanitize=undefined
        -fno-sanitize-recover
        -fno-omit-frame-pointer
    )
    target_link_options(test_posix_port PRIVATE
        -fsanitize=address
        -fsanitize=undefined
        -fno-sanitize-recover
    )
endif()

add_test(NAME posix_port COMMAND test_posix_port)
set_tests_properties(posix_port PROPERTIES TIMEOUT 15)

add_executable(test_uart_faults
    test_uart_faults.c
    ../../ports/uart/u_port_uart_linux.c
)
target_include_directories(test_uart_faults PRIVATE ../../ports)
target_compile_options(test_uart_faults PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wconversion
    -Wsign-conversion
    -Wshadow
    -pedantic
)
target_link_libraries(test_uart_faults PRIVATE util)
target_link_options(test_uart_faults PRIVATE
    -Wl,--wrap=malloc
    -Wl,--wrap=tcgetattr
    -Wl,--wrap=tcsetattr
    -Wl,--wrap=poll
    -Wl,--wrap=read
    -Wl,--wrap=write
)
if(ENABLE_COVERAGE)
    target_compile_options(test_uart_faults PRIVATE --coverage -O0 -g)
    target_link_options(test_uart_faults PRIVATE --coverage)
endif()

add_test(NAME uart_faults COMMAND test_uart_faults)
set_tests_properties(uart_faults PROPERTIES TIMEOUT 5)
