project(host)

if(${BUILD_TARGET} MATCHES "A200")
    add_definitions(-DATLAS_DEVICE)
else()
    add_definitions(-DATLAS_HOST)
endif()

####################################### config the host compile #############################
set(HOST_TARGET ObjectDetection)
set(HOST_INCUDUE_DIR
    ${DDK_INCLUDE_DIRS}
    ${DDK_CEREAL_INCLUDE_DIRS}
    ${DDK_PROTOBUF_INCLUDE_DIRS}
    ${PROJECT_SRC_ROOT}/ 
    ${PROJECT_SRC_ROOT}/DstEngine/ 
    ${PROJECT_SRC_ROOT}/common)

file(GLOB SRC ${PROJECT_SRC_ROOT}/DstEngine/*.cpp ${PROJECT_SRC_ROOT}/*.cpp)
set(HOST_SOURCE_FILE
    ${SRC})

set(HOST_SHARED_LIB_DIR
    )

set(HOST_SHARED_LIB 
    ${DDK_HOST_LIBRARIES}  
    ${DDK_PROTOBUF_LIBRARYS} 
    pthread )

####################################### host compile #############################
link_directories(${HOST_SHARED_LIB_DIR})
add_executable(${HOST_TARGET} ${HOST_SOURCE_FILE} ${PROJECT_SRC_ROOT}/../Common/CommandParser.h)
target_include_directories(${HOST_TARGET} PUBLIC ${HOST_INCUDUE_DIR})
target_link_libraries(${HOST_TARGET} ${HOST_SHARED_LIB})