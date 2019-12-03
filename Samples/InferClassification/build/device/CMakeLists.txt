project(device)

include(${PROJECT_SRC_ROOT}/../CMake/Ascend.cmake)
aux_source_directory(${PROJECT_SRC_ROOT}/../Common/CropResize CropResize_SRC)
aux_source_directory(${PROJECT_SRC_ROOT}/../Common/DvppJpegEncode DvppJpegEncode_SRC)

include_directories(${PROJECT_SRC_ROOT}/../Common/)
aux_source_directory(${PROJECT_SRC_ROOT}/JpegDecode JpegDecode_SRC)
add_library(JpegDecode SHARED ${JpegDecode_SRC} ${CropResize_SRC})
target_include_directories(JpegDecode PUBLIC ${PROJECT_SRC_ROOT}/JpegDecode ${PROJECT_SRC_ROOT}/../Common/CropResize)
target_link_libraries(JpegDecode ${DDK_DEVICE_LIBRARIES})

aux_source_directory(${PROJECT_SRC_ROOT}/PngDecode PngDecode_SRC)
add_library(PngDecode SHARED ${PngDecode_SRC} ${CropResize_SRC})
target_include_directories(PngDecode PUBLIC ${PROJECT_SRC_ROOT}/PngDecode ${PROJECT_SRC_ROOT}/../Common/CropResize)
target_link_libraries(PngDecode ${DDK_DEVICE_LIBRARIES})

aux_source_directory(${PROJECT_SRC_ROOT}/ObjectClassification ObjectClassification_SRC)
add_library(ObjectClassification SHARED ${ObjectClassification_SRC})
target_include_directories(ObjectClassification PUBLIC ${PROJECT_SRC_ROOT}/ObjectClassification)
target_link_libraries(ObjectClassification ${DDK_DEVICE_LIBRARIES})

aux_source_directory(${PROJECT_SRC_ROOT}/JpegEncode JpegEncode_SRC)
add_library(JpegEncode SHARED ${JpegEncode_SRC} ${DvppJpegEncode_SRC})
target_include_directories(JpegEncode PUBLIC ${PROJECT_SRC_ROOT}/JpegEncode ${PROJECT_SRC_ROOT}/../Common/DvppJpegEncode)
target_link_libraries(JpegEncode ${DDK_DEVICE_LIBRARIES})

