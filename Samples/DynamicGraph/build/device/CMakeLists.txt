project(device)

include(${PROJECT_SRC_ROOT}/../CMake/Ascend.cmake)

aux_source_directory(${PROJECT_SRC_ROOT}/GreetEngine GreetEngine_SRC)
add_library(Device SHARED ${GreetEngine_SRC})
target_include_directories(Device PUBLIC ${PROJECT_SRC_ROOT}/GreetEngine)
target_link_libraries(Device ${DDK_DEVICE_LIBRARIES})