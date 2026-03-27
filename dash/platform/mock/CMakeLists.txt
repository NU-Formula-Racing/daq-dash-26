# mock platform

# all .cpp files in this dir
# recursive glob
file(GLOB PLATFORM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

target_sources(dash_platform PRIVATE ${PLATFORM_SOURCES})