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

#ifndef ATLASFACEDEMO_STREAMPULLER_H
#define ATLASFACEDEMO_STREAMPULLER_H

#include "common_data_type.h"
#include "hiaiengine/engine.h"
#include <atomic>
#include <mutex>
#include <thread>

extern "C" {
#include "libavformat/avformat.h"
}

#define RP_INPUT_SIZE 1
#define RP_OUTPUT_SIZE 1

class StreamPuller : public hiai::Engine {
public:
    HIAI_StatusT Init(const hiai::AIConfig& config, const std::vector<hiai::AIModelDescription>& model_desc);

    HIAI_DEFINE_PROCESS(RP_INPUT_SIZE, RP_OUTPUT_SIZE)

    ~StreamPuller();

private:
    // todo
    // ?
    void getStreamInfo();
    void pullStreamDataLoop();
    void stopStream();
    HIAI_StatusT startStream(const string& streamName);

    // class member
    std::shared_ptr<AVFormatContext> pFormatCtx;
    // stream info
    uint64_t blockId = 0;
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t channelId = 0;
    uint32_t format = H264;
    int videoIndex;
    std::atomic<int> stop = { 0 };
    std::thread sendDataRunner;
    RawDataBufferHigh dataBuffer;
    uint64_t curBlockId = 0;
    std::string streamName;
};

#endif