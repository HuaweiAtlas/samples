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
#include <stdio.h>
#include "DataRegister.h"

using namespace std;
using namespace hiai;

shared_ptr<RawDataBuffer> g_picInData = make_shared<RawDataBuffer>();
shared_ptr<ResultInfoT> g_resultInfo = make_shared<ResultInfoT>();
shared_ptr<JpegEncodeInfo> g_jpgEncodeInfo = make_shared<JpegEncodeInfo>();
uint32_t g_decodeCount;
std::mutex g_mt;

HIAI_StatusT SrcEngine::Init(const hiai::AIConfig& config, const  std::vector<hiai::AIModelDescription>& model_desc)
{
    return HIAI_OK;
}

HIAI_IMPL_ENGINE_PROCESS("SrcEngine", SrcEngine, INPUT_SIZE)
{
    int ret = HIAI_OK;
    shared_ptr<GraphCtrlInfoT> ctrlInfo = static_pointer_cast<GraphCtrlInfoT>(arg0);
    if (arg0 == NULL) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "arg0 is null");
        return HIAI_ERROR;
    }
    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[SrcEngine] start");

    g_jpgEncodeInfo->width = ctrlInfo->width;
    g_jpgEncodeInfo->height = ctrlInfo->height;
    g_jpgEncodeInfo->format = ctrlInfo->format;
    g_jpgEncodeInfo->level = ctrlInfo->level;

    g_resultInfo->chipId = ctrlInfo->chipId;
    g_resultInfo->graphId = ctrlInfo->graphId;
    g_resultInfo->dEngineCount = ctrlInfo->dEngineCount;
    g_resultInfo->decodeCount = ctrlInfo->decodeCount;
    g_resultInfo->failedCount = 0;

    g_picInData->len_of_byte = ctrlInfo->bufferSize;
    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[SrcEngine] bufSize: %d", ctrlInfo->bufferSize);
    g_picInData->data.reset(ctrlInfo->dataBuff.get(), HIAIMemory::HIAI_DFree);

    g_decodeCount = ctrlInfo->decodeCount;
    
    gettimeofday(&(g_resultInfo->startTime), NULL);

    for (unsigned int i=0; i<ctrlInfo->decodeCount; i++) {
        std::shared_ptr<std::string> strData(new std::string);
        ret = SendData(i%ctrlInfo->dEngineCount, "string", static_pointer_cast<void>(strData));
        if (ret != HIAI_OK) {
            HIAI_ENGINE_LOG("[SrcEngine]SendData fail!ret = %d", ret);
            g_mt.lock();
            g_decodeCount--;
            g_mt.unlock();
        }
    }

    return ret;
}
