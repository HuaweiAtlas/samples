project(device)

# build for device
# HelloDavinci

include(${PROJECT_SRC_ROOT}/../CMake/Ascend.cmake)

aux_source_directory(${PROJECT_SRC_ROOT}/HelloDavinci HelloDavinci_SRC)
add_library(Device SHARED ${HelloDavinci_SRC})
target_include_directories(Device PUBLIC ${PROJECT_SRC_ROOT}/HelloDavinci)
target_link_libraries(Device ${DDK_DEVICE_LIBRARIES})
