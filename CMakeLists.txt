
cmake_minimum_required(VERSION 3.13...3.27)

# Set the PICO board you're targeting
set(PICO_BOARD pico2_w)

# Include the Raspberry Pi Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Initialize the Raspberry Pi Pico SDK
project(blinky)

# Set the C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Add the PIO encoder library
add_subdirectory(pio_encoder)  # Add the pio_encoder library from the subfolder

# Define the main executable
add_executable(${PROJECT_NAME}
    main.cpp  # Include your main application code here
)

# Link the PIO encoder library with your main executable
target_link_libraries(${PROJECT_NAME} 
    pio_encoder   # Link the pio_encoder library
    pico_stdlib   # Link standard Pico library
    hardware_pio
    hardware_irq
    hardware_clocks
)

# Generate the PIO header file for quadrature_encoder.pio
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/pio_encoder/src/quadrature_encoder.pio)

# Enable USB output, disable UART output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

# Optional: create map/bin/hex/uf2 file in addition to ELF
# pico_add_extra_outputs(${PROJECT_NAME})

