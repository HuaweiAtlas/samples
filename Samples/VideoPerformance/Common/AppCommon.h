/**
 * ============================================================================
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Atlas Sample
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

#ifndef APPCOMMON_H
#define APPCOMMON_H

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <memory>
#include "hiaiengine/data_type_reg.h"

static const int H264 = 0;
static const int H265 = 1;
static const uint32_t TIME_THOUSAND = 1000;
static const uint32_t DVPP_STRIDE_WIDTH = 128;
static const uint32_t DVPP_STRIDE_HEIGHT = 16;
static const float YUV_BYTES = 1.5;

typedef struct GraphCtrlInfo {
    uint32_t chipId;
    uint32_t graphId;
    uint32_t dEngineCount;
    uint32_t decodeCount;
    uint32_t testType;
    uint32_t height;
    uint32_t width;
    uint32_t bufferSize;
    std::shared_ptr<uint8_t> dataBuff;
}GraphCtrlInfoT;

template<class Archive>
void serialize(Archive& ar, GraphCtrlInfoT& data)
{
    ar(data.chipId, data.graphId, data.dEngineCount, data.decodeCount, data.testType, 
        data.bufferSize, data.height, data.width);
    if (data.bufferSize > 0 && data.dataBuff.get() == nullptr) {
        data.dataBuff.reset(new uint8_t[data.bufferSize]);
    }
    ar(cereal::binary_data(data.dataBuff.get(), data.bufferSize * sizeof(uint8_t)));
}

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

extern std::shared_ptr<ResultInfoT> g_resultInfo;
extern uint32_t g_decodeCount;
extern std::mutex g_mt;

#endif  // DVPPRESIZE_APPCOMMON_H




