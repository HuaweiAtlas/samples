cmake_minimum_required(VERSION 2.8)
include(${CMAKE_CURRENT_LIST_DIR}/../CMake/Euler.cmake)
add_compile_options(-std=c++11 -fstack-protector-all -Wl,-z,relro,-z,now,-z,noexecstack)
project(HaSample)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(PROJECT_SRC_ROOT ${CMAKE_CURRENT_LIST_DIR})

include_directories(include)
aux_source_directory(src main_SRC)

add_executable(mbox-sample ${main_SRC})
target_link_libraries(mbox-sample pthread)
