# okay.cmake -- links to okay engine

# dash application
set(SOURCES
    ${OKAY_PROJECT_ROOT_DIR}/main.cpp
    ${OKAY_PROJECT_ROOT_DIR}/can/mock/can_imgui.cpp
)

set(INCLUDES
    ${OKAY_PROJECT_ROOT_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# add needed drivers
set(DRIVERS_DIR ${OKAY_PROJECT_ROOT_DIR}/drivers)

add_subdirectory(
    ${DRIVERS_DIR}/can
    ${CMAKE_CURRENT_BINARY_DIR}/dash_drivers_can
) # produces static lib nfr_canlib

# add platform-specific functionalities
add_subdirectory(
    ${OKAY_PROJECT_ROOT_DIR}/platform
    ${CMAKE_CURRENT_BINARY_DIR}/dash_platform
)

target_sources(${PROJECT} PRIVATE ${SOURCES})
target_link_libraries(${PROJECT} PRIVATE dash_platform)
target_include_directories(${PROJECT} PRIVATE ${INCLUDES})

