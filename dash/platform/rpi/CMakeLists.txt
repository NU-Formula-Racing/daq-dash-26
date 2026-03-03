# rpi platform

find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIODCXX REQUIRED libgpiodcxx)

# neopixel library
set(NEOPIXEL_DIR ${DRIVERS_DIR}/neopixel)
add_subdirectory(${NEOPIXEL_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/dash_drivers_neopixel)

target_sources(dash_platform PRIVATE gpio.cpp gpio_manager.cpp spi.cpp neopixel.cpp platform.cpp)

target_include_directories(dash_platform PRIVATE ${GPIODCXX_INCLUDE_DIRS})
target_link_directories(dash_platform PRIVATE ${GPIODCXX_LIBRARY_DIRS})
target_link_libraries(dash_platform PRIVATE ${GPIODCXX_LIBRARIES})
