project(host)

aux_source_directory(${PROJECT_SRC_ROOT} main_src)

aux_source_directory(${PROJECT_SRC_ROOT}/GreetEngine GreetEngine_SRC)
add_executable(main ${main_src} ${GreetEngine_SRC})
target_include_directories(main PUBLIC ${DDK_PROTOBUF_INCLUDE_DIRS})
target_link_libraries(main ${DDK_HOST_LIBRARIES} ${DDK_PROTOBUF_LIBRARYS})