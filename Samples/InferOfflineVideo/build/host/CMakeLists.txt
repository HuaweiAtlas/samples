project(host)
 
if(${BUILD_TARGET} MATCHES "A200")
    add_definitions(-DATLAS_DEVICE)
else()
    add_definitions(-DATLAS_HOST)
endif()

if(NOT DEFINED ENV{FFMPEG_PATH})
    message(FATAL_ERROR "please define environment variable:FFMPEG_PATH")  
else()
    # message("FFMPEG_PATH $ENV{FFMPEG_PATH}")
    set(FFMPEG_PATH $ENV{FFMPEG_PATH})
endif()
find_package(FFMPEG REQUIRED)

include_directories(${PROJECT_SRC_ROOT}/../Common/FileManager)
aux_source_directory(${PROJECT_SRC_ROOT}/../Common/FileManager FileManager_SRC)

aux_source_directory(${PROJECT_SRC_ROOT}/StreamPuller StreamPuller_SRC)
add_library(StreamPuller SHARED ${StreamPuller_SRC})
target_include_directories(StreamPuller PUBLIC ${FFMPEG_INCLUDE_DIRS})
target_link_libraries(StreamPuller ${DDK_HOST_LIBRARIES} ${FFMPEG_LIBRARIES})

aux_source_directory(${PROJECT_SRC_ROOT}/DstEngine DstEngine_SRC)
add_library(DstEngine SHARED ${DstEngine_SRC})
target_include_directories(DstEngine PUBLIC ${FFMPEG_INCLUDE_DIRS})
target_link_libraries(DstEngine ${DDK_HOST_LIBRARIES})

aux_source_directory(${PROJECT_SRC_ROOT} main_src)
add_executable(main ${main_src} ${FileManager_SRC})
target_link_libraries(main ${DDK_HOST_LIBRARIES})
