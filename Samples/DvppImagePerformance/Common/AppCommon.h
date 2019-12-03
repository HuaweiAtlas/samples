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

#ifndef APP_COMMON_H
#define APP_COMMON_H

#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include "hiaiengine/data_type.h"
#include "hiaiengine/data_type_reg.h"
#include <memory>

using namespace hiai;


#define USE_DEFINE_ERROR 0x6001

enum {
    HIAI_IDE_ERROR_CODE,
    HIAI_IDE_INFO_CODE,
    HIAI_IDE_WARNING_CODE
};

HIAI_DEF_ERROR_CODE(USE_DEFINE_ERROR, HIAI_ERROR, HIAI_IDE_ERROR, \
    "");
HIAI_DEF_ERROR_CODE(USE_DEFINE_ERROR, HIAI_INFO, HIAI_IDE_INFO, \
    "");
HIAI_DEF_ERROR_CODE(USE_DEFINE_ERROR, HIAI_WARNING, HIAI_IDE_WARNING, \
    "");


enum TestType {
    DECODE_PIC_JPEG = 0,
    DECODE_PIC_PNG,
    ENCODE_PIC_JPEG,
    DECODE_VIDEO_H264,
    DECODE_VIDEO_H265
};

typedef struct GraphCtrlInfo {
    uint32_t chipId;
    uint32_t graphId;
    uint32_t dEngineCount;
    uint32_t decodeCount;
    TestType testType;
    uint32_t bufferSize;
    std::shared_ptr<uint8_t> dataBuff;

    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint32_t level;
}GraphCtrlInfoT;

typedef struct ResultInfo {
    uint32_t chipId;
    uint32_t graphId;
    uint32_t dEngineCount;
    uint32_t decodeCount;
    uint32_t failedCount;
    struct timeval startTime;
    struct timeval endTime;
}ResultInfoT;
template<class Archive>
void serialize(Archive& ar, ResultInfo& data)
{
    ar(data.chipId,
       data.graphId,
       data.dEngineCount,
       data.decodeCount,
       data.failedCount,
       data.startTime.tv_sec,
       data.startTime.tv_usec,
       data.endTime.tv_sec,
       data.endTime.tv_usec);
}

typedef struct JpegEncodeInfo {
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint32_t level;
}JpegEncodeInfoT;
template<class Archive>
void serialize(Archive &ar, JpegEncodeInfo &data)
{
    ar(data.width,
       data.height,
       data.format,
       data.level);
}
typedef struct CommandPara{
    bool help;
    std::string fileName;
    int chipStartNo;
    int chipEndNo;
    int graphPerChip;
    int threadPerGraph;
    int decodeType;
    int width;
    int height;
    int format;
    int level;
}CommandParaT;

extern std::shared_ptr<RawDataBuffer> g_picInData;
extern std::shared_ptr<ResultInfoT> g_resultInfo;
extern std::shared_ptr<JpegEncodeInfo> g_jpgEncodeInfo;
extern uint32_t g_decodeCount;
extern std::mutex g_mt;

#endif
