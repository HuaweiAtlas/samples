project(device)

include(${PROJECT_SRC_ROOT}/../CMake/Ascend.cmake)

include_directories(${PROJECT_SRC_ROOT}/../Common/)
include_directories(${DDK_OPENCV_INCLUDE_DIRS})

aux_source_directory(${PROJECT_SRC_ROOT}/VDecEngine VDecEngine_SRC)
aux_source_directory(${PROJECT_SRC_ROOT}/VencEngine VDecEngine_SRC)
aux_source_directory(${PROJECT_SRC_ROOT}/SrcEngine VDecEngine_SRC)
add_library(Device SHARED ${VDecEngine_SRC})
target_include_directories(Device PUBLIC ${PROJECT_SRC_ROOT}/VDecEngine)
target_include_directories(Device PUBLIC ${PROJECT_SRC_ROOT}/VencEngine)
target_include_directories(Device PUBLIC ${PROJECT_SRC_ROOT}/SrcEngine)
target_link_libraries(Device ${DDK_DEVICE_LIBRARIES})

