cmake_minimum_required(VERSION 2.8)
add_compile_options(-std=c++11 -fstack-protector-all -Wl,-z,relro,-z,now,-z,noexecstack)
project(EncodeVideo)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(PROJECT_SRC_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)
set(CMAKE_MODULE_PATH ${PROJECT_SRC_ROOT}/../CMake/)

# ddk
find_package(DDK REQUIRED)
include_directories(${DDK_INCLUDE_DIRS} ${DDK_CEREAL_INCLUDE_DIRS} ${DDK_PROTOBUF_INCLUDE_DIRS})

# common

include_directories(${PROJECT_SRC_ROOT}/../Common/)
include_directories(${PROJECT_SRC_ROOT}/common/)
add_subdirectory(host)
add_subdirectory(device)