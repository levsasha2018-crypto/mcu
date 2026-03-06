cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(03_adc C CXX ASM)

pico_sdk_init()

add_executable(${PROJECT_NAME}
    main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/led-task/led-task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/adc-task/adc-task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stdio-task/stdio-task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol-task/protocol-task.c
)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_adc
)

pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

pico_add_extra_outputs(${PROJECT_NAME})