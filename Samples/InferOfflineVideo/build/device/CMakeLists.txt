project(device)

include(${PROJECT_SRC_ROOT}/../CMake/Ascend.cmake)

include_directories(${PROJECT_SRC_ROOT}/../Common/)
include_directories(${DDK_OPENCV_INCLUDE_DIRS})
aux_source_directory(${PROJECT_SRC_ROOT}/../Common/DvppJpegEncode DvppJpegEncode_SRC)

aux_source_directory(${PROJECT_SRC_ROOT}/VDecEngine VDecEngine_SRC)
add_library(VDecEngine SHARED ${VDecEngine_SRC})
target_include_directories(VDecEngine PUBLIC ${PROJECT_SRC_ROOT}/VDecEngine)
target_link_libraries(VDecEngine ${DDK_DEVICE_LIBRARIES})

aux_source_directory(${PROJECT_SRC_ROOT}/SSDDetection SSDDetection_SRC)
add_library(SSDDetection SHARED ${SSDDetection_SRC})
target_include_directories(SSDDetection PUBLIC ${PROJECT_SRC_ROOT}/SSDDetection)
target_link_libraries(SSDDetection ${DDK_DEVICE_LIBRARIES} ${DDK_DEVICE_OPENCV_LIBRARYS})

aux_source_directory(${PROJECT_SRC_ROOT}/ObjectClassification ObjectClassification_SRC)
add_library(ObjectClassification SHARED ${ObjectClassification_SRC})
target_include_directories(ObjectClassification PUBLIC ${PROJECT_SRC_ROOT}/ObjectClassification)
target_link_libraries(ObjectClassification ${DDK_DEVICE_LIBRARIES})

aux_source_directory(${PROJECT_SRC_ROOT}/JpegEncode JpegEncode_SRC)
add_library(JpegEncode SHARED ${JpegEncode_SRC} ${DvppJpegEncode_SRC})
target_include_directories(JpegEncode PUBLIC ${PROJECT_SRC_ROOT}/JpegEncode ${PROJECT_SRC_ROOT}/../Common/DvppJpegEncode)
target_link_libraries(JpegEncode ${DDK_DEVICE_LIBRARIES})

