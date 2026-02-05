# okay.cmake -- links to okay engine

# dash application
set(SOURCES
    ${OKAY_PROJECT_ROOT_DIR}/main.cpp
)

set(INCLUDES
    ${OKAY_PROJECT_ROOT_DIR}
)

# add needed drivers
# for now only CAN
set(DRIVERS_DIR ${OKAY_PROJECT_ROOT_DIR}/drivers)
add_subdirectory(${DRIVERS_DIR}/can) # produces a static library nfr_canlib

# add platform-specific functionalities
add_subdirectory(${OKAY_PROJECT_ROOT_DIR}/platform)

# add the sources and includes to PROJECT executable
target_sources(${PROJECT} PRIVATE ${SOURCES})
target_include_directories(${PROJECT} PRIVATE ${INCLUDES})