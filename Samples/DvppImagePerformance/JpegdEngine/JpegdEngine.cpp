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

#include "JpegdEngine.h"
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

HIAI_StatusT JpegdEngine::Init(const AIConfig& config, 
    const std::vector<AIModelDescription>& model_desc)
{
    CreateDvppApi(dvppApi);
    if (NULL == dvppApi) {
        HIAI_ENGINE_LOG("jpegDecode engine: createDvppapi fail");
        return HIAI_ERROR;
    }
    return HIAI_OK;
}

JpegdEngine::~JpegdEngine()
{
    DestroyDvppApi(dvppApi);
}

void JpegdEngine::EndTestIfNeed()
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

HIAI_IMPL_ENGINE_PROCESS("JpegdEngine", JpegdEngine, INPUT_SIZE)
{
    int ret = HIAI_OK;
    struct jpegd_raw_data_info jpegdInData;
    struct jpegd_yuv_data_info jpegdOutData;

    jpegdInData.IsYUV420Need = true;
    jpegdInData.jpeg_data_size = g_picInData->len_of_byte + 8;    // the buf len should 8 byte larger, the driver asked

    void *addrOrig = HIAI_DVPP_DMalloc(jpegdInData.jpeg_data_size);
    if (addrOrig == nullptr) {
        HIAI_ENGINE_LOG(HIAI_ERROR, "can not alloc input buffer\n");
        g_resultInfo->failedCount++;
        EndTestIfNeed();
        return HIAI_ERROR;
    }
    jpegdInData.jpeg_data = static_cast<unsigned char*>(addrOrig);
    if (jpegdInData.jpeg_data == nullptr) {
        HIAI_DVPP_DFree(addrOrig);
        g_resultInfo->failedCount++;
        EndTestIfNeed();
        return HIAI_ERROR;
    }
    if (memcpy_s(jpegdInData.jpeg_data, jpegdInData.jpeg_data_size-8, g_picInData->data.get(), g_picInData->len_of_byte) != 0) {
        HIAI_DVPP_DFree(addrOrig);
        g_resultInfo->failedCount++;
        EndTestIfNeed();
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[JpegeEngine] memcpy inData buffer failed");
        return HIAI_ERROR;
    }

    dvppapi_ctl_msg dvppApiCtlMsg;
    dvppApiCtlMsg.in = static_cast<void *>(&jpegdInData);
    dvppApiCtlMsg.in_size = sizeof(jpegdInData);
    dvppApiCtlMsg.out = static_cast<void *>(&jpegdOutData);
    dvppApiCtlMsg.out_size = sizeof(jpegdOutData);

    if (DvppCtl(dvppApi, DVPP_CTL_JPEGD_PROC, &dvppApiCtlMsg) != 0) {
        HIAI_DVPP_DFree(addrOrig);
        HIAI_ENGINE_LOG(HIAI_ERROR, "dvpp process error");
        g_resultInfo->failedCount++;
        EndTestIfNeed();
        return HIAI_ERROR;
    }

    HIAI_DVPP_DFree(addrOrig);
    EndTestIfNeed();

    return ret;
}
