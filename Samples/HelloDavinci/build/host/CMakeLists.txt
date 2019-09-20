project(host)

# copy graph.config file
file(COPY ${PROJECT_SRC_ROOT}/graph.config
        DESTINATION ${CMAKE_BINARY_DIR})

include_directories(${PROJECT_SRC_ROOT}/../Common/GraphManager)
aux_source_directory(${PROJECT_SRC_ROOT}/../Common/GraphManager GraphManager_SRC)
include_directories(${PROJECT_SRC_ROOT}/../Common/FileManager)
aux_source_directory(${PROJECT_SRC_ROOT}/../Common/FileManager FileManager_SRC)
aux_source_directory(${PROJECT_SRC_ROOT}/SrcEngine SrcEngine_SRC)
aux_source_directory(${PROJECT_SRC_ROOT}/DstEngine DstEngine_SRC)
aux_source_directory(${PROJECT_SRC_ROOT} main_SRC)

add_executable(main ${main_SRC} ${GraphManager_SRC} ${FileManager_SRC} ${SrcEngine_SRC} ${DstEngine_SRC})
target_include_directories(main PUBLIC ${DDK_PROTOBUF_INCLUDE_DIRS})
target_link_libraries(main ${DDK_HOST_LIBRARIES} ${DDK_PROTOBUF_LIBRARYS})

