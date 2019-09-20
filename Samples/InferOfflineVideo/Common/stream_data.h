/**
 * ============================================================================
 *
 * Copyright (C) 2019, Huawei Technologies Co., Ltd. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1 Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   2 Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   3 Neither the names of the copyright holders nor the names of the
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * ============================================================================
 */
#ifndef ATLASFACEDEMO_STREAM_DATA_H
#define ATLASFACEDEMO_STREAM_DATA_H

#include "RawDataBufferHigh.h"
#include "cereal/types/utility.hpp"
#include "hiaiengine/data_type.h"
#include "hiaiengine/data_type_reg.h"
#include <array>
#include <chrono>
#include <memory>
#include <time.h>
#include <unordered_map>
#include <vector>

typedef struct timespec Time;
template <class Archive>
void serialize(Archive& ar, Time& data)
{
    ar(data.tv_sec, data.tv_nsec);
}
using std::unordered_map;
using time_pair = std::pair<Time, Time>;
using std::string;
using time_table = unordered_map<string, time_pair>;
using hiai::Point2D;
using hiai::Rectangle;
using std::array;
using std::shared_ptr;
using std::vector;

#define NUM_FEATURES 512
#define NUM_ANGLES 3
#define NUM_ATTRIBUTE 2
#define MAX_FACE_NUM 32

hiai::RawDataBuffer a;

typedef struct ClassifyResult
{
    int classIndex;
    float confidence;
}ClassifyResultT;
template<class Archive>
void serialize(Archive& ar, ClassifyResultT& classifyResultT)
{
    ar(classifyResultT.classIndex, classifyResultT.confidence);

}

struct StreamInfo {
    uint64_t frameId; /* frame id, in video stream channel */
    uint32_t mode; /* Operate mode: register, normal, decode H26* */
    uint32_t format; /* raw data format: jpg, png, h264, h265 */
    uint32_t channelId; /* video stream channel, corresponding to camera */
    uint32_t isEOS; /* flag of video stream reception */
    time_table table;
};
template <class Archive>
void serialize(Archive& ar, StreamInfo& data)
{
    ar(data.frameId, data.mode, data.format, data.channelId, data.isEOS, data.table);
}

struct StreamRawData {
    StreamInfo info;
    RawDataBufferHigh buf;
};
template <class Archive>
void serialize(Archive& ar, StreamRawData& data)
{
    ar(data.info, data.buf);
}

struct ImageInfo {
    uint32_t format;
    uint32_t width;
    uint32_t height;
    uint32_t widthAligned;
    uint32_t heightAligned;
    RawDataBufferHigh buf; // data buffer for image
};
template <class Archive>
void serialize(Archive& ar, ImageInfo& data)
{
    ar(data.format, data.width, data.height, data.widthAligned, data.heightAligned, data.buf);
}

struct DetectInfo {
    int32_t classId;
    float confidence;
    Rectangle<Point2D> location;
    ClassifyResultT classifyResult;
};
template <class Archive>
void serialize(Archive& ar, DetectInfo& data)
{
    ar(data.classId, data.confidence, data.location, data.classifyResult);
}

struct FaceObject {
    uint64_t id;
    uint32_t isKeyFrame = 0;
    DetectInfo info;
    vector<Point2D> landmarks;
    vector<float> featureVec;
    vector<float> angles;
    vector<float> attributes; // gender, age, quality
    ImageInfo imgOrigin;
    ImageInfo imgCroped;
    ImageInfo imgAffine;
};
template <class Archive>
void serialize(Archive& ar, FaceObject& data)
{
    //    ar(data.id, data.isKeyFrame, data.info, data.landmarks, data.featureVec, data.angles, data.attributes, data.imgCroped);
    ar(data.id, data.isKeyFrame, data.info, data.landmarks, data.featureVec, data.angles, data.attributes);
}

struct FaceOutputInfo {
    StreamInfo info;
    shared_ptr<FaceObject> face;
};
template <class Archive>
void serialize(Archive& ar, FaceOutputInfo& data)
{
    ar(data.info, data.face);
}

struct DeviceStreamData {
    enum Action {
        doAll = 0,
        doDetection = 1,
        doLandmark = 2,
        doNothing = 3,
    };
    StreamInfo info;
    Action action = doAll;
    vector<DetectInfo> detectResult;
    vector<shared_ptr<FaceObject> > faces;
    ImageInfo imgOrigin;
};
template <class Archive>
void serialize(Archive& ar, DeviceStreamData& data)
{
    ar(data.info, data.detectResult, data.faces, data.imgOrigin);
    // ar(data.info, data.detectResult, data.faces);
}

#define USE_HPC_DATA 1
#if USE_HPC_DATA

// StreamRawData
inline void StreamRawDataSerialize(void* input, std::string& control, std::uint8_t*& data, std::uint32_t& data_length)
{
    auto streamData = static_cast<StreamRawData*>(input);
    control = std::string(static_cast<char*>(input), sizeof(StreamRawData));
    data = streamData->buf.data.get();
    data_length = streamData->buf.len_of_byte;
}
inline std::shared_ptr<void> StreamRawDataDeserialize(const char* control, const std::uint32_t& control_length, const std::uint8_t* data, const std::uint32_t& data_length)
{
    auto streamData = std::make_shared<StreamRawData>();
    streamData->info = reinterpret_cast<StreamRawData*>(const_cast<char*>(control))->info;
    streamData->buf.len_of_byte = reinterpret_cast<StreamRawData*>(const_cast<char*>(control))->buf.len_of_byte;

    if (0 != streamData->buf.len_of_byte) {
        streamData->buf.data = std::shared_ptr<std::uint8_t>(const_cast<std::uint8_t*>(data), hiai::Graph::ReleaseDataBuffer);
    }

    return std::static_pointer_cast<void>(streamData);
}
HIAI_REGISTER_SERIALIZE_FUNC("StreamRawData", StreamRawData, StreamRawDataSerialize, StreamRawDataDeserialize);

HIAI_REGISTER_DATA_TYPE("FaceOutputInfo", FaceOutputInfo);
HIAI_REGISTER_DATA_TYPE("DeviceStreamData", DeviceStreamData);

#else // USE_HPC_DATA

#endif // USE_HPC_DATA

#endif //ATLASFACEDEMO_STREAM_DATA_H
