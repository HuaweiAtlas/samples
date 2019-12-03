project(host)

include_directories(${PROJECT_SRC_ROOT}/../Common/)
include_directories(${PROJECT_SRC_ROOT}/../Common/GraphManager)
include_directories(${PROJECT_SRC_ROOT}/../Common/FileManager)
aux_source_directory(${PROJECT_SRC_ROOT}/../Common/GraphManager GraphManager_SRC)
aux_source_directory(${PROJECT_SRC_ROOT}/../Common/FileManager FileManager_SRC)
aux_source_directory(${PROJECT_SRC_ROOT}/MindInputFile MindInputFile_SRC)
aux_source_directory(${PROJECT_SRC_ROOT}/DstEngine DstEngine_SRC)
aux_source_directory(${PROJECT_SRC_ROOT} main_SRC)

add_executable(main ${main_SRC} ${GraphManager_SRC} ${FileManager_SRC} ${MindInputFile_SRC} ${DstEngine_SRC})
target_link_libraries(main ${DDK_HOST_LIBRARIES} ${DDK_PROTOBUF_LIBRARYS})

