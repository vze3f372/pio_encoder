
cmake_minimum_required(VERSION 3.13)

# Add the PIO source files
add_library(pio_encoder
    src/EncoderRingBuffer.cpp
    src/GpioPin.cpp
    src/QuadratureEncoder.cpp
    src/led.cpp
    src/quadrature_encoder.pio
)

# Set include directories for the generated header
target_include_directories(pio_encoder PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

# Generate the PIO header for the quadrature_encoder.pio file
pico_generate_pio_header(pio_encoder ${CMAKE_CURRENT_LIST_DIR}/src/quadrature_encoder.pio)

# Link required libraries
target_link_libraries(pio_encoder pico_stdlib hardware_pio hardware_irq hardware_clocks)

