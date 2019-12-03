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

#include "VencEngine.h"
#include "hiaiengine/data_type.h"
#include "hiaiengine/data_type_reg.h"
#include "hiaiengine/log.h"
#include "hiaiengine/ai_memory.h"
#include "dvpp/Venc.h"
#include "AppCommon.h"

#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cstring>
using namespace std;

HIAI_StatusT VencEngine::Init(const hiai::AIConfig& config, const std::vector<hiai::AIModelDescription>& model_desc)
{
    return HIAI_OK;
}

std::string g_vencOutFileName("venc.bin");
std::shared_ptr<FILE> g_vencOutFile(nullptr);
void VencEngine::VencCallBackDumpFile(struct VencOutMsg* vencOutMsg, void* userData)
{
    if (g_vencOutFile.get() == nullptr) {
        return;
    }
    VencEngine* vencPtr = NULL;
    if (userData != NULL) {
        vencPtr = (VencEngine*)userData;
    } else {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "userData is NULL");
        return;
    }
    fwrite(vencOutMsg->outputData, 1, vencOutMsg->outputDataSize, g_vencOutFile.get());
    fflush(g_vencOutFile.get());
    g_mt.lock();
    g_decodeCount--;
    if (g_decodeCount == 0) {
        gettimeofday(&(g_resultInfo->endTime), NULL);
        HIAI_StatusT ret = vencPtr->SendData(0, "ResultInfoT", std::static_pointer_cast<void>(g_resultInfo));
        if (HIAI_OK != ret) {
            HIAI_ENGINE_LOG(HIAI_ERROR, "SendData failed! ret = %d", ret);
        }
    }
    g_mt.unlock();
}

HIAI_IMPL_ENGINE_PROCESS("VencEngine", VencEngine, INPUT_SIZE)
{
    std::shared_ptr<GraphCtrlInfo> inputArg = std::static_pointer_cast<GraphCtrlInfo>(arg0);
    struct VencConfig vencConfig;
    vencConfig.width = inputArg->width;
    vencConfig.height = inputArg->height;
    vencConfig.codingType = inputArg->testType;
    vencConfig.yuvStoreType = 0;
    vencConfig.keyFrameInterval = 1;
    vencConfig.vencOutMsgCallBack = VencEngine::VencCallBackDumpFile;
    vencConfig.userData = this;
    if (vencHandle == 0) {
        vencHandle = CreateVenc(&vencConfig);
        if (vencHandle == -1) {
            HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "CreateVenc failed");
            return HIAI_ERROR;
        } 
    }

    char c[PATH_MAX + 1] = {0x00};
    errno_t err = strcpy_s(c, PATH_MAX + 1, g_vencOutFileName.c_str());
    if (err != EOK) {
        printf("[VideoEncode ERROR] strcpy %s failed!\n", c);
        return HIAI_ERROR;
    }

    char path[PATH_MAX + 1] = {0x00};
    if (realpath(c, path) == NULL) {
        printf("Writing file %s\n", path);
    }
    g_vencOutFile.reset(fopen(path, "wb"), [](FILE* p){fclose(p);});
    if (g_vencOutFile.get() == NULL) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "open out file failed");
        return HIAI_ERROR;
    }
    if (g_decodeCount == 0) {
        g_decodeCount = inputArg->decodeCount + 1;
        gettimeofday(&(g_resultInfo->startTime), NULL);
    }
    for (int i = 0; i < inputArg->decodeCount; i++) {
        struct VencInMsg vencInMsg;
        vencInMsg.inputData = (void*)(inputArg->dataBuff.get());
        vencInMsg.inputDataSize = (uint32_t)(inputArg->bufferSize);
        vencInMsg.keyFrameInterval = 0;
        vencInMsg.forceIFrame = 0;
        vencInMsg.eos = 0;

        if (RunVenc(vencHandle, &vencInMsg) == -1) {
            HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "RunVenc failed");
            DestroyVenc(vencHandle);
            return HIAI_ERROR;
        }
    }

    DestroyVenc(vencHandle);
    
    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[VideoEncode INFO] process end");
    
    return HIAI_OK;
} 

