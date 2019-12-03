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

#include "JpegeEngine.h"
#include <memory>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include "hiaiengine/log.h"
#include "hiaiengine/ai_memory.h"
#include "hiaiengine/c_graph.h"
#define MAP_VA32BIT 0x200

using namespace std;
using namespace hiai;

HIAI_StatusT JpegeEngine::Init(const AIConfig& config, 
    const std::vector<AIModelDescription>& model_desc)
{
    CreateDvppApi(dvppApi);
    if (NULL == dvppApi) {
        HIAI_ENGINE_LOG("jpegDecode engine: createDvppapi fail");
        return HIAI_ERROR;
    }
    return HIAI_OK;
}

JpegeEngine::~JpegeEngine()
{
    DestroyDvppApi(dvppApi);
}

void JpegeEngine::EndTestIfNeed()
{
    int ret;
    g_mt.lock();
    g_decodeCount--;
    if (g_decodeCount == 0) {
        gettimeofday(&(g_resultInfo->endTime), NULL);
        ret = SendData(0, "ResultInfoT", std::static_pointer_cast<void>(g_resultInfo));
        if (ret != HIAI_OK) {
            HIAI_ENGINE_LOG("SendData failed! ret = %d", ret);
        }
    }
    g_mt.unlock();
}

HIAI_IMPL_ENGINE_PROCESS("JpegeEngine", JpegeEngine, INPUT_SIZE)
{
    int ret = HIAI_OK;
    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[JpegeEngine] start");
   
    sJpegeIn  inData;
    sJpegeOut outData;
    inData.width         = g_jpgEncodeInfo->width;
    inData.height        = g_jpgEncodeInfo->height;
    inData.heightAligned = inData.height;
    inData.format        = (eEncodeFormat)g_jpgEncodeInfo->format;
    inData.level         = g_jpgEncodeInfo->level;
    
    if (g_jpgEncodeInfo->format == 0 || g_jpgEncodeInfo->format == 1 || g_jpgEncodeInfo->format == 2 || g_jpgEncodeInfo->format == 3) {
        inData.stride = ALIGN_UP(inData.width * 2, 16);
        inData.bufSize = ALIGN_UP(inData.stride * inData.heightAligned, PAGE_SIZE);
    } else {
        inData.stride = ALIGN_UP(inData.width, 16);
        inData.bufSize = ALIGN_UP(inData.stride * inData.heightAligned * 3 / 2, PAGE_SIZE);
    }

    void *addrOrig = HIAI_DVPP_DMalloc(inData.bufSize);
    if (addrOrig == nullptr) {
        g_resultInfo->failedCount++;
        EndTestIfNeed();
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[JpegeEngine] can not alloc input buffer");
        return HIAI_ERROR;
    }
    inData.buf = static_cast<unsigned char*>(addrOrig);
    if (inData.buf == nullptr) {
        HIAI_DVPP_DFree(addrOrig);
        g_resultInfo->failedCount++;
        EndTestIfNeed();
        return HIAI_ERROR;
    }
    if (memcpy_s(inData.buf, g_picInData->len_of_byte, g_picInData->data.get(), g_picInData->len_of_byte) != 0) {
        HIAI_DVPP_DFree(addrOrig);
        g_resultInfo->failedCount++;
        EndTestIfNeed();
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[JpegeEngine] memcpy inData buffer failed");
        return HIAI_ERROR;
    }

    dvppapi_ctl_msg dvppApiCtlMsg;
    dvppApiCtlMsg.in = static_cast<void *>(&inData);
    dvppApiCtlMsg.in_size = sizeof(inData);
    dvppApiCtlMsg.out = static_cast<void *>(&outData);
    dvppApiCtlMsg.out_size = sizeof(outData);

    if (DvppCtl(dvppApi, DVPP_CTL_JPEGE_PROC, &dvppApiCtlMsg) != 0) {
        HIAI_DVPP_DFree(addrOrig);
        DestroyDvppApi(dvppApi);
        g_resultInfo->failedCount++;
        EndTestIfNeed();
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[JpegeEngine] dvppCtl error");
        return HIAI_ERROR;
    }
    HIAI_DVPP_DFree(addrOrig);
    EndTestIfNeed();
    return ret;
}
