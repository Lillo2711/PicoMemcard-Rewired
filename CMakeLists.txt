cmake_minimum_required(VERSION 3.13)

if(NOT DEFINED PICO_BOARD)
    set(PICO_BOARD pico CACHE STRING "Board type")
endif()

include(pico_sdk_import.cmake)

project(picomemcard_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(PicoMemcard)

pico_set_linker_script(PicoMemcard ${CMAKE_SOURCE_DIR}/memmap.ld)
pico_generate_pio_header(PicoMemcard ${CMAKE_CURRENT_LIST_DIR}/psxSPI.pio)
pico_generate_pio_header(PicoMemcard ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

# Escolhe driver de LED conforme a placa
if(DEFINED PICO_ZERO)
    set(LED_SRC ${CMAKE_SOURCE_DIR}/src/led.c)
    target_compile_definitions(PicoMemcard PUBLIC PICO=1)
else()
    set(LED_SRC ${CMAKE_SOURCE_DIR}/src/led_ws2812.c)
    target_compile_definitions(PicoMemcard PUBLIC PICO_ZERO=1)
endif()
message(STATUS "LED driver: ${LED_SRC}")

target_sources(PicoMemcard PUBLIC
    ${LED_SRC}
    ${CMAKE_SOURCE_DIR}/src/main.c
    ${CMAKE_SOURCE_DIR}/src/memcard_manager.c
    ${CMAKE_SOURCE_DIR}/src/memcard_simulator.c
    ${CMAKE_SOURCE_DIR}/src/memory_card.c
    ${CMAKE_SOURCE_DIR}/src/msc_handler.c
    ${CMAKE_SOURCE_DIR}/src/sd_config.c
    ${CMAKE_SOURCE_DIR}/src/usb_descriptors.c
)

# Example include
target_include_directories(PicoMemcard PUBLIC
    ${CMAKE_SOURCE_DIR}/inc
)

pico_enable_stdio_uart(PicoMemcard 1)	# enable only UART stdio

add_subdirectory(no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)
target_link_libraries(PicoMemcard pico_stdlib pico_multicore pico_time hardware_pio tinyusb_device tinyusb_board FatFs_SPI)


pico_add_extra_outputs(PicoMemcard)