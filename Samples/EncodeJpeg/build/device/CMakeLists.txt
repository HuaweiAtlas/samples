project(device)

# build for device
# HelloDavinci
include(${PROJECT_SRC_ROOT}/../CMake/Ascend.cmake)
include_directories(${PROJECT_SRC_ROOT}/../Common/)

aux_source_directory(${PROJECT_SRC_ROOT}/JpegEncode JpegEncode_SRC)
aux_source_directory(${PROJECT_SRC_ROOT}/../Common/DvppJpegEncode DvppJpegEncode_SRC)
message("EncodeJpeg is ${PROJECT_SRC_ROOT}")
add_library(Device SHARED ${JpegEncode_SRC} ${DvppJpegEncode_SRC})
target_include_directories(Device PUBLIC ${PROJECT_SRC_ROOT}/EncodeJpeg ${PROJECT_SRC_ROOT}/../Common/DvppJpegEncode)
target_link_libraries(Device ${DDK_DEVICE_LIBRARIES} ${DDK_DEVICE_LIBRARIES})

