# okay.cmake -- links dash application to okay engine

message(STATUS "OKAY_PROJECT_ROOT_DIR = ${OKAY_PROJECT_ROOT_DIR}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "PROJECT = ${PROJECT}")

set(SOURCES
    "${OKAY_PROJECT_ROOT_DIR}/main.cpp"
    "${OKAY_PROJECT_ROOT_DIR}/can/mock/can_imgui.cpp"
)

target_sources(${PROJECT} PRIVATE
    ${SOURCES}
)

target_include_directories(${PROJECT} PRIVATE
    "${OKAY_PROJECT_ROOT_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Add needed drivers
set(DRIVERS_DIR "${OKAY_PROJECT_ROOT_DIR}/drivers")

add_subdirectory(
    "${DRIVERS_DIR}/can"
    "${CMAKE_CURRENT_BINARY_DIR}/dash_drivers_can"
)

# Add platform-specific functionality
add_subdirectory(
    "${OKAY_PROJECT_ROOT_DIR}/platform"
    "${CMAKE_CURRENT_BINARY_DIR}/dash_platform"
)

# Add UI library
add_subdirectory(
    "${OKAY_PROJECT_ROOT_DIR}/ui"
    "${CMAKE_CURRENT_BINARY_DIR}/dash_ui"
)

target_link_libraries(${PROJECT} PRIVATE
    dash_platform
    dash_ui
)
