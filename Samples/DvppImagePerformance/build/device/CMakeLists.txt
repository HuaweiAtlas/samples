project(device)

include(${PROJECT_SRC_ROOT}/../CMake/Ascend.cmake)

include_directories(${PROJECT_SRC_ROOT}/../Common/)
include_directories(${DDK_OPENCV_INCLUDE_DIRS})

aux_source_directory(${PROJECT_SRC_ROOT}/JpegeEngine DVPP_SRC)
aux_source_directory(${PROJECT_SRC_ROOT}/JpegdEngine DVPP_SRC)
aux_source_directory(${PROJECT_SRC_ROOT}/PngdEngine DVPP_SRC)
aux_source_directory(${PROJECT_SRC_ROOT}/SrcEngine DVPP_SRC)
add_library(Device SHARED ${DVPP_SRC})
target_include_directories(Device PUBLIC ${PROJECT_SRC_ROOT}/JpegdEngine)
target_include_directories(Device PUBLIC ${PROJECT_SRC_ROOT}/JpegeEngine)
target_include_directories(Device PUBLIC ${PROJECT_SRC_ROOT}/PngdEngine)
target_include_directories(Device PUBLIC ${PROJECT_SRC_ROOT}/SrcEngine)
target_link_libraries(Device ${DDK_DEVICE_LIBRARIES})

