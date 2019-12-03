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

#include "DstEngine.h"
#include <sys/stat.h>
#include "AppCommon.h"

using Stat = struct stat;
HIAI_REGISTER_DATA_TYPE("ResultInfoT", ResultInfoT);
HIAI_StatusT DstEngine::Init(const hiai::AIConfig& config, const std::vector<hiai::AIModelDescription>& model_desc) 
{
    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "DstEngine init success");
    return HIAI_OK;
}

HIAI_IMPL_ENGINE_PROCESS("DstEngine", DstEngine, DST_INPUT_SIZE)
{
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[DstEngine]  start");
    if (arg0 == nullptr) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[DstEngine]  The input arg0 is nullptr");
        return HIAI_ERROR;
    }
    std::shared_ptr<ResultInfoT> resultInfo = std::static_pointer_cast<ResultInfoT>(arg0);
    double costMs = 1000.0*(resultInfo->endTime.tv_sec-resultInfo->startTime.tv_sec) + \
        (resultInfo->endTime.tv_usec-resultInfo->startTime.tv_usec)/1000.0;
    double fps = resultInfo->decodeCount*1000.0/costMs;
    printf("chip:%d\tgraph:%d\tthread:%d\tcount:%d\tfailed:%d\tcost:%fms\tfps:%f\n", resultInfo->chipId, 
        resultInfo->graphId, resultInfo->dEngineCount, resultInfo->decodeCount, 
        resultInfo->failedCount, costMs, fps);
    std::shared_ptr<std::string> srcData = std::make_shared<std::string>();
    SendData(0, "string", srcData);
    HIAI_ENGINE_LOG(HIAI_INFO, "[DstEngine] end process!");
    return HIAI_OK;
}
