project(device)

if(NOT ${BUILD_TARGET} MATCHES "A200")
    include(${PROJECT_SRC_ROOT}/../CMake/Ascend.cmake)
endif()

# build for device
# HelloDavinci

aux_source_directory(${PROJECT_SRC_ROOT}/HelloDavinci HelloDavinci_SRC)
add_library(Device SHARED ${HelloDavinci_SRC})
target_include_directories(Device PUBLIC ${PROJECT_SRC_ROOT}/HelloDavinci)
target_link_libraries(Device ${DDK_DEVICE_LIBRARIES})
