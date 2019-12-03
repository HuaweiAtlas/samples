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

#include "SrcEngine.h"
#include <hiaiengine/log.h>
#include "hiaiengine/ai_memory.h"
#include "error_code.h"
#include <stdio.h>
#include "AppCommon.h"

using namespace std;
using namespace hiai;
HIAI_REGISTER_DATA_TYPE("GraphCtrlInfoT", GraphCtrlInfo);
shared_ptr<ResultInfoT> g_resultInfo = make_shared<ResultInfoT>();
uint32_t g_decodeCount;
std::mutex g_mt;

HIAI_StatusT SrcEngine::Init(const hiai::AIConfig& config, const  std::vector<hiai::AIModelDescription>& model_desc)
{
    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "SrcEngine init success");
    return HIAI_OK;
}

HIAI_IMPL_ENGINE_PROCESS("SrcEngine", SrcEngine, INPUT_SIZE)
{
    int ret = HIAI_OK;
    shared_ptr<GraphCtrlInfo> inputArg = static_pointer_cast<GraphCtrlInfo>(arg0);

    g_resultInfo->chipId = inputArg->chipId;
    g_resultInfo->graphId = inputArg->graphId;
    g_resultInfo->dEngineCount = inputArg->dEngineCount;
    g_resultInfo->decodeCount = inputArg->decodeCount;
    g_resultInfo->failedCount = 0;
    for (unsigned int i=0; i<inputArg->dEngineCount; i++) {
        ret = SendData(i%inputArg->dEngineCount, "GraphCtrlInfo", static_pointer_cast<void>(inputArg));
        if (ret != HIAI_OK) {
            HIAI_ENGINE_LOG("[SrcEngine]SendData fail!ret = %d", ret);
            g_mt.lock();
            g_decodeCount--;
            g_mt.unlock();
        }
    }

    return ret;
}
