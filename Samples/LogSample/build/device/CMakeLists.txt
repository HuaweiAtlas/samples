project(device)

# build for device
# DeviceEngine

include(${PROJECT_SRC_ROOT}/../CMake/Ascend.cmake)

aux_source_directory(${PROJECT_SRC_ROOT}/DeviceEngine DeviceEngine_SRC)
add_library(Device SHARED ${DeviceEngine_SRC})
target_include_directories(Device PUBLIC ${PROJECT_SRC_ROOT}/DeviceEngine)
target_link_libraries(Device ${DDK_DEVICE_LIBRARIES})
