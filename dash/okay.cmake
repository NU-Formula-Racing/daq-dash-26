set(SOURCES
    ${OKAY_PROJECT_ROOT_DIR}/main.cpp
)

set(INCLUDES
    ${OKAY_PROJECT_ROOT_DIR}
)

# add the sources and includes to PROJECT executable
target_sources(${PROJECT} PRIVATE ${SOURCES})
target_include_directories(${PROJECT} PRIVATE ${INCLUDES})