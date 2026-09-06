cmake_minimum_required(VERSION 3.16)
project(u_connect_client_no_os_tests C)

include(CTest)

option(ENABLE_COVERAGE "Build with gcov instrumentation" ON)
option(ENABLE_SANITIZERS "Build with address and undefined sanitizers" ON)

add_executable(test_no_os_port
    test_no_os_port.c
    ../../ports/os/u_port_no_os.c
)
target_compile_definitions(test_no_os_port PRIVATE U_PORT_NO_OS)
target_include_directories(test_no_os_port PRIVATE
    ../../inc
    ../../ports
)
target_compile_options(test_no_os_port PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wconversion
    -Wsign-conversion
    -Wshadow
    -pedantic
)
target_link_options(test_no_os_port PRIVATE -Wl,--wrap=clock_gettime)

if(ENABLE_COVERAGE)
    target_compile_options(test_no_os_port PRIVATE --coverage -O0 -g)
    target_link_options(test_no_os_port PRIVATE --coverage)
endif()

if(ENABLE_SANITIZERS)
    target_compile_options(test_no_os_port PRIVATE
        -fsanitize=address
        -fsanitize=undefined
        -fno-sanitize-recover
        -fno-omit-frame-pointer
    )
    target_link_options(test_no_os_port PRIVATE
        -fsanitize=address
        -fsanitize=undefined
        -fno-sanitize-recover
    )
endif()

add_test(NAME no_os_boot_at_zero COMMAND test_no_os_port boot-at-zero)
add_test(NAME no_os_day_rollover COMMAND test_no_os_port day-rollover)
add_test(NAME no_os_timer_wrap COMMAND test_no_os_port timer-wrap)
add_test(NAME no_os_sleep COMMAND test_no_os_port sleep)
add_test(NAME no_os_mutex_and_rx COMMAND test_no_os_port mutex-and-rx)
set_tests_properties(
    no_os_boot_at_zero
    no_os_day_rollover
    no_os_timer_wrap
    no_os_sleep
    no_os_mutex_and_rx
    PROPERTIES TIMEOUT 5
)
