# rpi platform

find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)

# neopixel library
set(NEOPIXEL_DIR ${DRIVERS_DIR}/neopixel)
add_subdirectory(${NEOPIXEL_DIR})

target_sources(dash_platform PRIVATE gpio.cpp spi.cpp)

target_include_directories(dash_platform PRIVATE ${GPIOD_INCLUDE_DIRS})
target_link_directories(dash_platform PRIVATE ${GPIOD_LIBRARY_DIRS})
target_link_libraries(dash_platform PRIVATE ${GPIOD_LIBRARIES})