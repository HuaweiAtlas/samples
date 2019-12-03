project(device)

# build for device
# ObjectDetection

if(NOT ${BUILD_TARGET} MATCHES "A200")    
    include(${PROJECT_SRC_ROOT}/../CMake/Ascend.cmake)
else()
    include(${PROJECT_SRC_ROOT}/../CMake/A200SOC.cmake)
endif()

################################### config the device compile ##############################
set(DEVICE_TARGET Device)
set(DEVICE_INCUDUE_DIR
    ${DDK_INCLUDE_DIRS}
    ${DDK_PROTOBUF_INCLUDE_DIRS}
    ${DDK_CEREAL_INCLUDE_DIRS}
    ${PROJECT_SRC_ROOT}/ObjectDetectionEngine
    ${PROJECT_SRC_ROOT}/DecodeEngine
    ${PROJECT_SRC_ROOT}/common)

aux_source_directory(${PROJECT_SRC_ROOT}/ObjectDetectionEngine ObjectDetectionEngine_SRC)
aux_source_directory(${PROJECT_SRC_ROOT}/DecodeEngine DecodeEngine_SRC)
set(DEVICE_SOURCE_FILE
    ${ObjectDetectionEngine_SRC}
    ${DecodeEngine_SRC})

set(DEVICE_SHARED_LIB_DIR
    )

set(DEVICE_SHARED_LIB 
    ${DDK_DEVICE_LIBRARIES})

################################### device compile ##############################
link_directories(${DEVICE_SHARED_LIB_DIR})
add_library(${DEVICE_TARGET} SHARED ${DEVICE_SOURCE_FILE})
target_include_directories(${DEVICE_TARGET} PUBLIC ${DEVICE_INCUDUE_DIR})
target_link_libraries(${DEVICE_TARGET} ${DEVICE_SHARED_LIB})
