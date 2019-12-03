project(device)

# build for device
# HelloDavinci

include(${PROJECT_SRC_ROOT}/../CMake/Ascend.cmake)
include_directories(${PROJECT_SRC_ROOT}/../Common/)

aux_source_directory(${PROJECT_SRC_ROOT}/VideoEncode VideoEncode_SRC)
add_library(Device SHARED ${VideoEncode_SRC})
target_include_directories(Device PUBLIC ${PROJECT_SRC_ROOT}/VideoEncode)
target_link_libraries(Device ${DDK_DEVICE_LIBRARIES})