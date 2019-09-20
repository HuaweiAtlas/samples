project(device)

# build for device
# HelloDavinci
include(${PROJECT_SRC_ROOT}/../CMake/Ascend.cmake)
include_directories(${PROJECT_SRC_ROOT}/../Common/)

aux_source_directory(${PROJECT_SRC_ROOT}/DvppCrop DvppCrop_SRC)
aux_source_directory(${PROJECT_SRC_ROOT}/../Common/CropResize CropResize_SRC)

add_library(Device SHARED ${DvppCrop_SRC} ${CropResize_SRC})
target_include_directories(Device PUBLIC ${PROJECT_SRC_ROOT}/DvppCrop ${PROJECT_SRC_ROOT}/../Common/CropResize)
target_link_libraries(Device ${DDK_DEVICE_LIBRARIES} ${DDK_DEVICE_LIBRARIES})

