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

#include <unistd.h>
#include <thread>
#include <hiaiengine/data_type.h>
#include <vector>
#include <memory>
#include <fstream>
#include <iostream>

#include "DstEngine.h"
#include "common_data_type.h"
#include "error_code.h"

HIAI_REGISTER_DATA_TYPE("StreamInfo", StreamInfo);
HIAI_REGISTER_DATA_TYPE("ImageInfo", ImageInfo);
HIAI_REGISTER_DATA_TYPE("DetectInfo", DetectInfo);
HIAI_REGISTER_DATA_TYPE("DeviceStreamData", DeviceStreamData);

HIAI_StatusT DstEngine::Init(const hiai::AIConfig &config,
                             const std::vector<hiai::AIModelDescription> &model_desc)
{
    HIAI_ENGINE_LOG(HIAI_INFO, "DstEngine Init");

    HIAI_ENGINE_LOG(HIAI_INFO, "DstEngine init success");
    return HIAI_OK;
}

// Display Engine
HIAI_IMPL_ENGINE_PROCESS("DstEngine", DstEngine, POST_PROCESS_INPUT_SIZE)
{
    HIAI_ENGINE_LOG(APP_INFO, " DstEngine engine begin");

    std::shared_ptr<DeviceStreamData> inputArg = std::static_pointer_cast<DeviceStreamData>(arg0);
    if (nullptr == inputArg) {
        HIAI_ENGINE_LOG(APP_ERROR, "fail to process invalid message");
        return HIAI_ERROR;
    }

    if (inputArg->detectResult.size()) {
        printf("[INF] channel %d, frame %ld have %ld object", inputArg->info.channelId, inputArg->info.frameId,
               inputArg->detectResult.size());

        for (int i = 0; i < inputArg->detectResult.size(); i++) {
            printf(" #%d, bbox(%4d, %4d, %4d, %4d)", i, inputArg->detectResult[i].location.anchor_lt.x,
                   inputArg->detectResult[i].location.anchor_lt.y,
                   inputArg->detectResult[i].location.anchor_rb.x, inputArg->detectResult[i].location.anchor_rb.y);
        }
        printf("\n");
    }

    if (inputArg->info.isEOS == 1) {
        /* crop small image for follow up process */
        if (HIAI_OK != hiai::Engine::SendData(0, "DeviceStreamData",
                                              std::static_pointer_cast<void>(inputArg))) {
            HIAI_ENGINE_LOG(APP_ERROR, "ObjectDetectionEngine senddata error!");
            return HIAI_ERROR;
        }
    }
    HIAI_ENGINE_LOG(APP_INFO, "DstEngine engine end");
    return HIAI_OK;
}
