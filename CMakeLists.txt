cmake_minimum_required(VERSION 3.13)

if(NOT DEFINED PICO_BOARD)
    set(PICO_BOARD pico CACHE STRING "Board type")
endif()

include(pico_sdk_import.cmake)

project(picomemcard_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

# ===========================
# Seleção da placa
# ===========================

if(DEFINED PICO_1)
    set(FW_NAME PicoMemCard-MH_PICO_1)
    set(LED_SRC ${CMAKE_SOURCE_DIR}/src/led.c)
    set(BOARD_DEF PICO_1=1)
else()
    set(FW_NAME PicoMemCard-MH_PICO_0)
    set(BOARD_DEF PICO_0=1)

    if(DEFINED SET_GP_LED)
        set(LED_SRC ${CMAKE_SOURCE_DIR}/src/led.c)
    else()
        set(LED_SRC ${CMAKE_SOURCE_DIR}/src/led_ws2812.c)
    endif()
endif()

# ===========================
# Target
# ===========================

add_executable(${FW_NAME})

target_sources(${FW_NAME} PUBLIC
    ${LED_SRC}
    ${CMAKE_SOURCE_DIR}/src/main.c
    ${CMAKE_SOURCE_DIR}/src/memcard_manager.c
    ${CMAKE_SOURCE_DIR}/src/memcard_simulator.c
    ${CMAKE_SOURCE_DIR}/src/memory_card.c
    ${CMAKE_SOURCE_DIR}/src/msc_handler.c
    ${CMAKE_SOURCE_DIR}/src/sd_config.c
    ${CMAKE_SOURCE_DIR}/src/usb_descriptors.c
)

# ===========================
# Defines comuns
# ===========================

target_compile_definitions(${FW_NAME} PUBLIC ${BOARD_DEF})

if(DEFINED SET_GP_LED)
    target_compile_definitions(${FW_NAME} PUBLIC SET_GP_LED=${SET_GP_LED})
    message(STATUS "LED GPIO: ${SET_GP_LED}")
endif()

message(STATUS "LED driver: ${LED_SRC}")
message(STATUS "Firmware: ${FW_NAME}")


pico_set_linker_script(${FW_NAME} ${CMAKE_SOURCE_DIR}/memmap.ld)
pico_generate_pio_header(${FW_NAME} ${CMAKE_CURRENT_LIST_DIR}/psxSPI.pio)
pico_generate_pio_header(${FW_NAME} ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

target_include_directories(${FW_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/inc
)

pico_enable_stdio_uart(${FW_NAME} 1)	# enable only UART stdio

#add_subdirectory(no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)
add_subdirectory(no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI FatFs_SPI_build)

target_link_libraries(${FW_NAME} pico_stdlib pico_multicore pico_time hardware_pio tinyusb_device tinyusb_board FatFs_SPI)

pico_add_extra_outputs(${FW_NAME})