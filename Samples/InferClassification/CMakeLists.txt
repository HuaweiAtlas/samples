cmake_minimum_required(VERSION 3.5)
project(InferClassification)

set(CMAKE_CXX_STANDARD 14)

include_directories(Common)
include_directories(Common/DvppJpegEncode)
include_directories(DstEngine)
include_directories(JpegEncode)
include_directories(ObjectClassification)
include_directories(SSDDetection)
include_directories(StreamPuller)
include_directories(VDecEngine)

add_executable(InferOfflineVideo
        Common/DvppJpegEncode/DvppJpegEncode.cpp
        Common/DvppJpegEncode/DvppJpegEncode.h
        Common/Common.h
        Common/common_data_type.h
        Common/DataRecv.h
        Common/dvpp_utils.h
        Common/engine_tools.h
        Common/error_code.h
        Common/RawDataBufferHigh.h
        Common/stream_data.h
        Common/TransSear.h
        Common/utils_common.h
        DstEngine/DstEngine.cpp
        DstEngine/DstEngine.h
        JpegEncode/JpegEncode.cpp
        JpegEncode/JpegEncode.h
        ObjectClassification/ObjectClassification.cpp
        ObjectClassification/ObjectClassification.h
        SSDDetection/SSDDetection.cpp
        SSDDetection/SSDDetection.h
        StreamPuller/StreamPuller.cpp
        StreamPuller/StreamPuller.h
        VDecEngine/VDecEngine.cpp
        VDecEngine/VDecEngine.h
        main.cpp)
