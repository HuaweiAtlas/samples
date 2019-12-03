project(host)
 
aux_source_directory(${PROJECT_SRC_ROOT} main_src)
aux_source_directory(${PROJECT_SRC_ROOT}/../Common/FileManager main_src)
aux_source_directory(${PROJECT_SRC_ROOT}/DstEngine main_src)
aux_source_directory(${PROJECT_SRC_ROOT}/DynamicGraph main_src)
add_executable(main ${main_src})
target_link_libraries(main ${DDK_HOST_LIBRARIES} ${DDK_PROTOBUF_LIBRARYS} ${DDK_OPENCV_LIBRARYS})
