cmake_minimum_required(VERSION 2.8)
add_compile_options(-std=c++11)
project(HelloDavinci)

if(NOT DEFINED BUILD_TARGET)
    message(FATAL_ERROR "please use -DBUILD_TARGET=[DEVICE|HOST]")  
endif()

# ddk
find_package(DDK REQUIRED)
include_directories(${DDK_INCLUDE_DIRS} ${DDK_CEREAL_INCLUDE_DIRS} ${DDK_PROTOBUF_INCLUDE_DIRS})

# common
include_directories(./Common/include)

if(${BUILD_TARGET} STREQUAL DEVICE)

    # build for device
    # HelloDavinci
    aux_source_directory(HelloDavinci HelloDavinci_SRC)
    add_library(HelloDavinci SHARED ${HelloDavinci_SRC})
    target_include_directories(HelloDavinci PUBLIC HelloDavinci)
    target_link_libraries(HelloDavinci ${DDK_DEVICE_LIBRARIES})

elseif(${BUILD_TARGET} STREQUAL HOST)

    # build for host
    include_directories(./SrcEngine)
    include_directories(./DstEngine)
    aux_source_directory(. SRC)
    aux_source_directory(SrcEngine SRC)
    aux_source_directory(DstEngine SRC)
    add_executable(main ${SRC})
    target_link_libraries(main ${DDK_HOST_LIBRARIES})

endif()
