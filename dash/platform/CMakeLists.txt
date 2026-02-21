# platform/CMakeLists.txt

add_library(dash_platform STATIC)

target_include_directories(dash_platform PUBLIC
    ${OKAY_PROJECT_ROOT_DIR}
)

target_link_libraries(dash_platform PUBLIC nfr_canlib okay)

if(NOT DEFINED OKAY_PLATFORM)
    if(UNIX AND NOT APPLE)
        set(OKAY_PLATFORM "rpi")
    endif()
endif()

if(OKAY_PLATFORM STREQUAL "rpi")
    message(STATUS "Platform is rpi")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/rpi)
else()
    message(STATUS "Platform is mock")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mock)
endif()
