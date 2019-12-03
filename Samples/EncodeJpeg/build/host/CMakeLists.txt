project(host)

# copy graph.config file
file(COPY ${PROJECT_SRC_ROOT}/graph.config
        DESTINATION ${CMAKE_BINARY_DIR})

include_directories(${PROJECT_SRC_ROOT}/../Common/GraphManager)
include_directories(${PROJECT_SRC_ROOT}/../Common/FileManager)
aux_source_directory(${PROJECT_SRC_ROOT}/InputFile InputFile_SRC)
aux_source_directory(${PROJECT_SRC_ROOT}/SaveFile SaveFile_SRC)
aux_source_directory(${PROJECT_SRC_ROOT}/../Common/GraphManager GraphManager_SRC)
aux_source_directory(${PROJECT_SRC_ROOT}/../Common/FileManager FileManager_SRC)
aux_source_directory(${PROJECT_SRC_ROOT} main_SRC)

add_executable(main ${main_SRC} ${GraphManager_SRC} ${FileManager_SRC} ${InputFile_SRC} ${SaveFile_SRC})
target_include_directories(main PUBLIC ${DDK_PROTOBUF_INCLUDE_DIRS})
target_link_libraries(main ${DDK_HOST_LIBRARIES})
target_link_libraries(main ${DDK_HOST_LIBRARIES} ${DDK_PROTOBUF_LIBRARYS})

