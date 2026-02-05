# platform/CMakeLists.txt

add_library(dash_platform STATIC)

# include the can library
target_link_libraries(dash_platform PUBLIC nfr_canlib)

# if OKAY_PLATFORM is rpi, use add the rpi directory, otherwise use mock
if (OKAY_PLATFORM STREQUAL "rpi")
    message(STATUS "Platform is rpi")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/rpi)
else()
    message(STATUS "Platform is mock")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mock)
endif()