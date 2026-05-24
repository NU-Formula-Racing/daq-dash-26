# ui/CMakeLists.txt

add_library(dash_ui STATIC)

file(GLOB_RECURSE DASH_UI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

target_sources(dash_ui PRIVATE
    ${DASH_UI_SOURCES}
)

target_include_directories(dash_ui PUBLIC
    "${OKAY_PROJECT_ROOT_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(dash_ui PUBLIC
    okay
    nfr_canlib
)
