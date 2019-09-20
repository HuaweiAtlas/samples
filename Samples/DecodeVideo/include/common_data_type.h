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
#ifndef ATLASSAMPLES_COMMON_DATA_TYPE_H
#define ATLASSAMPLES_COMMON_DATA_TYPE_H

#include "EngineTransNew.h"

#define H264 0
#define H265 1
#ifndef YUV_BYTES
#define YUV_BYTES 1.5
#endif

struct StreamInfo {
    uint64_t frameId;   /* frame id, in video stream channel */
    uint32_t mode;      /* Operate mode: register, normal, decode H26**/
    uint32_t format;    /* raw data format: jpg, png, h264, h265 */
    uint32_t channelId; /* video stream channel, corresponding to camera */
    uint32_t isEOS;     /* flag of video stream reception */
    uint32_t width;
    uint32_t height;
};

struct StreamRawData {
    StreamInfo info;
    EngineTransNewT buf;
};

#define USE_HPC_DATA 1
#if USE_HPC_DATA

/**
* @ingroup hiaiengine
* @brief GetTransSearPtr, ���л�Trans����
* @param [in] : dataPtr �ṹ��ָ��
* @param [out]��structStr �ṹ��buffer
* @param [out]��buffer �ṹ������ָ��buffer
* @param [out]��dataSize �ṹ�����ݴ�С
 */
inline void GetStreamRawDataSearPtr(void *dataPtr, std::string &structStr, uint8_t *&buffer, uint32_t &bufferSize)
{
    StreamRawData *engineTrans = (StreamRawData *)dataPtr;
    structStr = std::string((const char *)dataPtr, sizeof(StreamRawData));
    buffer = (uint8_t *)engineTrans->buf.transBuff.get();
    bufferSize = engineTrans->buf.bufferSize;
}
/**
* @ingroup hiaiengine
* @brief GetTransSearPtr, �����л�Trans����
* @param [in] : ctrl_ptr �ṹ��ָ��
* @param [in] : ctrlLen ���ݽṹ�п�����Ϣ��С
* @param [in] : data_ptr �ṹ������ָ��
* @param [in] : dataLen �ṹ��������Ϣ�洢�ռ��С��������У�飬����ʾԭʼ������Ϣ��С
* @param [out]��std::shared_ptr<void> ����Engine��ָ��ṹ��ָ��
 */
inline std::shared_ptr<void> GetStreamRawDataDearPtr(const char *ctrlPtr, const uint32_t &ctrlLen,
                                                     const unsigned char *dataPtr, const uint32_t &dataLen)
{
    std::shared_ptr<StreamRawData> engineTransPtr = std::make_shared<StreamRawData>();
    engineTransPtr->buf.bufferSize = ((StreamRawData *)ctrlPtr)->buf.bufferSize;
    engineTransPtr->buf.transBuff.reset((unsigned char *)dataPtr, hiai::Graph::ReleaseDataBuffer);
    engineTransPtr->info.frameId = ((StreamRawData *)ctrlPtr)->info.frameId;
    engineTransPtr->info.mode = ((StreamRawData *)ctrlPtr)->info.mode;
    engineTransPtr->info.format = ((StreamRawData *)ctrlPtr)->info.format;
    engineTransPtr->info.channelId = ((StreamRawData *)ctrlPtr)->info.channelId;
    engineTransPtr->info.isEOS = ((StreamRawData *)ctrlPtr)->info.isEOS;
    engineTransPtr->info.width = ((StreamRawData *)ctrlPtr)->info.width;
    engineTransPtr->info.height = ((StreamRawData *)ctrlPtr)->info.height;
    return std::static_pointer_cast<void>(engineTransPtr);
}

// HIAI_REGISTER_SERIALIZE_FUNC("StreamRawData", StreamRawData, GetStreamRawDataSearPtr, GetStreamRawDataDearPtr);

#else

template<class Archive>
void serialize(Archive &ar, StreamInfo &data)
{
    ar(data.frameId, data.mode, data.format, data.channelId, data.isEOS, data.width, data.height);
}

template<class Archive>
void serialize(Archive &ar, EngineTransNewT &data)
{
    ar(data.bufferSize);
    if (data.bufferSize > 0 && data.transBuff.get() == nullptr) {
        data.transBuff.reset(new uint8_t[data.bufferSize]);
    }
    ar(cereal::binary_data(data.transBuff.get(), data.bufferSize * sizeof(uint8_t)));
}

template<class Archive>
void serialize(Archive &ar, StreamRawData &data)
{
    ar(data.info, data.buf);
}

HIAI_REGISTER_DATA_TYPE("StreamRawData", StreamRawData);

#endif

#endif  // ATLASSAMPLES_COMMON_DATA_TYPE_H
