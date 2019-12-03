cmake_minimum_required(VERSION 2.8)
add_compile_options(-std=c++11 -fstack-protector-all -Wl,-z,relro,-z,now,-z,noexecstack)
project(ObjectDetection)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(PROJECT_SRC_ROOT ${CMAKE_CURRENT_LIST_DIR}/../)
set(CMAKE_MODULE_PATH ${PROJECT_SRC_ROOT}/../CMake/)

# ddk
find_package(DDK REQUIRED)

################################### set compile and target ##############################
add_subdirectory(host)
add_subdirectory(device)

################################### other operate ##############################
# copy graph.config file
file(COPY ${PROJECT_SRC_ROOT}/graph.config
        DESTINATION ${CMAKE_BINARY_DIR})
