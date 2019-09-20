project(host)

aux_source_directory(${PROJECT_SRC_ROOT}/SrcEngine SrcEngine_SRC)
aux_source_directory(${PROJECT_SRC_ROOT}/DstEngine DstEngine_SRC)
aux_source_directory(${PROJECT_SRC_ROOT} main_src)


add_executable(main ${main_src} ${SrcEngine_SRC} ${DstEngine_SRC})
target_include_directories(main PUBLIC ${DDK_PROTOBUF_INCLUDE_DIRS})
target_link_libraries(main ${DDK_HOST_LIBRARIES} ${DDK_PROTOBUF_LIBRARYS})