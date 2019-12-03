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

#include "PngdEngine.h"
#include <memory>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include "hiaiengine/log.h"
#include "hiaiengine/ai_memory.h"
#define MAP_VA32BIT 0x200

using namespace std;
using namespace hiai;

HIAI_StatusT PngdEngine::Init(const AIConfig& config,
    const vector<AIModelDescription>& model_desc)
{
    CreateDvppApi(pIDVPPAPI);
    if (NULL == pIDVPPAPI) {
        HIAI_ENGINE_LOG(HIAI_ERROR, "pngDecode engine: createDvppapi fail");
        return HIAI_ERROR;
    }
    return HIAI_OK;
}

PngdEngine::~PngdEngine()
{
    DestroyDvppApi(pIDVPPAPI);
}

void PngdEngine::EndTestIfNeed()
{
    int ret;
    g_mt.lock();
    g_decodeCount--;
    if (g_decodeCount == 0) {
        gettimeofday(&(g_resultInfo->endTime), NULL);
        ret = SendData(0, "ResultInfoT", static_pointer_cast<void>(g_resultInfo));
        if (ret != HIAI_OK) {
            HIAI_ENGINE_LOG(HIAI_ERROR, "SendData failed! ret = %d", ret);
        }
    }
    g_mt.unlock();
}

HIAI_IMPL_ENGINE_PROCESS("PngdEngine", PngdEngine, INPUT_SIZE)
{
    int ret = HIAI_OK;
    struct PngInputInfoAPI pngInData;
    struct PngOutputInfoAPI pngOutData;

    pngInData.inputData = (unsigned char *)(g_picInData->data.get());
    pngInData.inputSize = g_picInData->len_of_byte;
    pngInData.transformFlag = 1; // RGBA -> RGB

    dvppapi_ctl_msg dvppApiCtlMsg;
    dvppApiCtlMsg.in = static_cast<void *>(&pngInData);
    dvppApiCtlMsg.in_size = sizeof(pngInData);
    dvppApiCtlMsg.out = static_cast<void *>(&pngOutData);
    dvppApiCtlMsg.out_size = sizeof(pngOutData);

    if (DvppCtl(pIDVPPAPI, DVPP_CTL_PNGD_PROC, &dvppApiCtlMsg) != 0) {
        HIAI_ENGINE_LOG(HIAI_ERROR, "dvpp process error");
        g_resultInfo->failedCount++;
        EndTestIfNeed();
        return HIAI_ERROR;
    }
    EndTestIfNeed();

    return ret;
}
