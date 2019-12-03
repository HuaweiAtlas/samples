project(device)

if(NOT ${BUILD_TARGET} MATCHES "A200")    
    include(${PROJECT_SRC_ROOT}/../CMake/Ascend.cmake)
endif()

add_definitions(-DATLAS_DEVICE)

# copy graph.config file
file(COPY ${PROJECT_SRC_ROOT}/graph.config
        DESTINATION ${CMAKE_BINARY_DIR})

aux_source_directory(${PROJECT_SRC_ROOT}/VDecEngine VDecEngine_SRC)
add_library(Device SHARED ${VDecEngine_SRC})
target_include_directories(Device PUBLIC ${PROJECT_SRC_ROOT}/VDecEngine)
target_link_libraries(Device ${DDK_DEVICE_LIBRARIES})