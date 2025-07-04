# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_BOARD pico_w)
include(pico_sdk_import.cmake)

project(pico_w_wifi_example C CXX ASM)
pico_sdk_init()

add_executable(pico_w_wifi_example
    pico_w_wifi_example.c
)

pico_enable_stdio_usb(pico_w_wifi_example 1)
pico_enable_stdio_uart(pico_w_wifi_example 0)

target_link_libraries(pico_w_wifi_example
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    hardware_adc
)

target_include_directories(pico_w_wifi_example PRIVATE ${CMAKE_CURRENT_LIST_DIR})

pico_add_extra_outputs(pico_w_wifi_example)