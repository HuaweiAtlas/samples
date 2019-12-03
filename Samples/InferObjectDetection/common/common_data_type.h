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

#ifndef COMMON_DATA_TYPE_H_
#define COMMON_DATA_TYPE_H_

#include "dvpp/dvpp_config.h"
#include "hiaiengine/ai_memory.h"
#include "hiaiengine/data_type.h"
#include "hiaiengine/data_type_reg.h"
#include <vector>

using hiai::BatchInfo;
using hiai::ImageData;
using hiai::IMAGEFORMAT;
using namespace hiai;

const float  YUV_BYTES = 1.5;
const int BGR_BYTES = 3;
const int RGB_BYTES = 3;
const int ALIGN_16 = 16;
const int ALIGN_128 = 128;

enum STREAM_MODE {MODE_NORMAL = 0};
enum STREAM_FORMAT {FORMAT_JPG = 0, FORMAT_PNG, FORMAT_H264, FORMAT_H265};

/*
* define 
*/
struct StreamInfo {
    uint32_t mode;      /* Operate mode: normal:0, Don't wait for batch size: 1 */
    uint32_t format;    /* raw data format: jpg:0, png:1, h264:2, h265:3 */
    uint32_t channelId; /* video stream channel, corresponding to camera */
    uint64_t frameId;   /* frame id, in video stream channel */
    uint32_t isEOS;     /* flag of video stream reception */
};
template<class Archive>
void serialize(Archive &ar, StreamInfo &data)
{
    ar(data.mode, data.format, data.channelId, data.frameId, data.isEOS);
}

/* configuration parameter */
struct StreamConfig {
    uint32_t frameInterval; /* frame skip interval */
    uint32_t faceDetectionBatchSize;
    float faceDetectionThreshold;
};
template<class Archive>
void serialize(Archive &ar, StreamConfig &data)
{
    ar(data.frameInterval, data.faceDetectionBatchSize, data.faceDetectionThreshold);
}

/* send raw data from host to device: jpg, png, h264, h265 */
struct StreamRawData {
    RawDataBuffer buf;
    StreamInfo info;
    // StreamConfig config;
};
inline void StreamRawDataSerialize(void *input, std::string &control, std::uint8_t *&data,
                                   std::uint32_t &data_length)
{
    auto streamData = static_cast<StreamRawData *>(input);
    control = std::string(static_cast<char *>(input), sizeof(StreamRawData));
    data = streamData->buf.data.get();
    data_length = streamData->buf.len_of_byte;
}
inline std::shared_ptr<void> StreamRawDataDeserialize(const char *control, const std::uint32_t &control_length,
                                                      const std::uint8_t *data, const std::uint32_t &data_length)
{
    auto streamData = std::make_shared<StreamRawData>();
    streamData->info = reinterpret_cast<StreamRawData *>(const_cast<char *>(control))->info;
    streamData->buf.len_of_byte = reinterpret_cast<StreamRawData *>(const_cast<char *>(control))->buf.len_of_byte;

    if (streamData->buf.len_of_byte != 0) {
        streamData->buf.data = std::shared_ptr<std::uint8_t>(const_cast<std::uint8_t *>(data),
                                                             hiai::Graph::ReleaseDataBuffer);
    }

    return std::static_pointer_cast<void>(streamData);
}

/* image format and raw data */
struct ImageInfo {
    uint32_t format;
    uint32_t width;
    uint32_t height;
    uint32_t widthAligned;
    uint32_t heightAligned;
    uint32_t channel;

    RawDataBuffer buf;  // data buffer for image
};
template<class Archive>
void serialize(Archive &ar, ImageInfo &data)
{
    ar(data.format, data.width, data.height, data.widthAligned, data.heightAligned, data.channel, data.buf);
}

/* for detect result */
struct DetectInfo {
    int32_t classId;
    float confidence;
    Rectangle<Point2D> location;
};
template<class Archive>
void serialize(Archive &ar, DetectInfo &data)
{
    ar(data.classId, data.confidence, data.location);
}

/* Transfer data between different device engines */
struct DeviceStreamData {
    StreamInfo info;
    ImageInfo imgOrigin;
    ImageInfo detectImg;

    std::vector<DetectInfo> detectResult;
};
template<class Archive>
void serialize(Archive &ar, DeviceStreamData &data)
{
    ar(data.info, data.imgOrigin, data.detectImg, data.detectResult);
}

#endif
