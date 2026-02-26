# rpi platform

find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIODCXX REQUIRED libgpiodcxx)

target_sources(dash_platform PRIVATE gpio.cpp spi.cpp gpio_manager.cpp platform.cpp)

target_include_directories(dash_platform PRIVATE ${GPIODCXX_INCLUDE_DIRS})
target_link_directories(dash_platform PRIVATE ${GPIODCXX_LIBRARY_DIRS})
target_link_libraries(dash_platform PRIVATE ${GPIODCXX_LIBRARIES})
