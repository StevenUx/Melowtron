cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD "pico")

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(Proyecto_final)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(melowtron
	melowtron.c
	ws2812.c
	
)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)

# generate the header file into the source tree as it is included in the RP2040 datasheet
pico_generate_pio_header(melowtron ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)













target_include_directories(melowtron PRIVATE ${CMAKE_CURRENT_LIST_DIR})

pico_enable_stdio_usb(melowtron 1)
# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(melowtron pico_stdlib hardware_pio hardware_dma hardware_adc hardware_pwm hardware_sync hardware_timer hardware_gpio hardware_resets hardware_irq)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(melowtron)