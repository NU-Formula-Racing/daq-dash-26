# mock platform

target_sources(dash_platform PRIVATE 
    gpio.cpp 
    can.cpp 
    spi.cpp 
    neopixel.cpp 
    button.cpp 
    encoder.cpp 
    input_manager.cpp
    glfw_input.cpp
)
