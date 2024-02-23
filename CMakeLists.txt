cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(avionics_ai C CXX ASM)

set(CMAKE_C_COMPILER /usr/bin/arm-none-eabi-gcc CACHE PATH "" FORCE)
set(CMAKE_CXX_COMPILER /usr/bin/arm-none-eabi-g++ CACHE PATH "" FORCE)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_subdirectory(bno055)
add_subdirectory(lcd_1_8)
include_directories(./bno055)
include_directories(./lcd_1_8)

add_executable(avionics_ai main.c)

pico_enable_stdio_usb(avionics_ai 1)
pico_enable_stdio_uart(avionics_ai 1)

pico_add_extra_outputs(avionics_ai)

target_link_libraries(avionics_ai pico_stdlib hardware_i2c pico_multicore bno055 lcd_1_8)
