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

#include "VideoEncode.h"
#include "hiaiengine/data_type.h"
#include "hiaiengine/data_type_reg.h"
#include "hiaiengine/log.h"
#include "hiaiengine/ai_memory.h"
#include "dvpp/Venc.h"

#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cstring>

#include "Common.h"

using namespace std;

HIAI_REGISTER_DATA_TYPE("EncodeVideoBlock", EncodeVideoBlock);

HIAI_StatusT VideoEncode::Init(const hiai::AIConfig &config,
                               const std::vector<hiai::AIModelDescription> &model_desc)
{
    std::stringstream ss;
    for (int index = 0; index < config.items_size(); ++index) {
        const ::hiai::AIConfigItem &item = config.items(index);
        std::string itemName = item.name();
        ss << item.value();
        if (itemName == "codingType") {
            ss >> codingType;
        }
        ss.clear();
    }

    return HIAI_OK;
}

void VideoEncode::VencCallBackDumpFile(struct VencOutMsg *vencOutMsg, void *userData)
{
    VideoEncode *vencPtr = NULL;
    if (userData != NULL) {
        vencPtr = (VideoEncode *)userData;
    } else {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[VideoEncode]userData is NULL");
        return;
    }

    std::shared_ptr<EncodeVideoBlock> sendDataPtr = std::make_shared<EncodeVideoBlock>();
    sendDataPtr->frameId = (vencPtr->frameID)++;
    sendDataPtr->encodedVideoFrame.len_of_byte = vencOutMsg->outputDataSize;
    sendDataPtr->encodedVideoFrame.data = std::shared_ptr<uint8_t>(new uint8_t[vencOutMsg->outputDataSize]);
    errno_t ret = memcpy_s(sendDataPtr->encodedVideoFrame.data.get(), sendDataPtr->encodedVideoFrame.len_of_byte,
                           vencOutMsg->outputData, vencOutMsg->outputDataSize);
    if (ret != EOK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[VideoEncode] memcpy_s() return error");
        return;
    }

    if (vencPtr->SendData(0, "EncodeVideoBlock",
                          std::static_pointer_cast<void>(sendDataPtr)) != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[Input INFO] SendData() error!");
        return;
    }
}

HIAI_IMPL_ENGINE_PROCESS("VideoEncode", VideoEncode, INPUT_SIZE)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "VideoEncode start");
    std::shared_ptr<EncodeVideoBlock> inputArg = std::static_pointer_cast<EncodeVideoBlock>(arg0);

    struct VencConfig vencConfig;
    vencConfig.width = inputArg->imageWidth;
    vencConfig.height = inputArg->imageHeight;
    vencConfig.codingType = codingType;
    vencConfig.yuvStoreType = 0;
    vencConfig.keyFrameInterval = 1;
    vencConfig.vencOutMsgCallBack = VencCallBackDumpFile;
    vencConfig.userData = this;

    int32_t vencHandle = CreateVenc(&vencConfig);
    if (vencHandle == -1) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[VideoEncode] CreateVenc failed");
        return HIAI_ERROR;
    }

    struct VencInMsg vencInMsg;
    vencInMsg.inputData = (void *)(inputArg->rawImage.data.get());
    vencInMsg.inputDataSize = (uint32_t)(inputArg->rawImage.len_of_byte);
    vencInMsg.keyFrameInterval = 0;
    vencInMsg.forceIFrame = 0;
    vencInMsg.eos = inputArg->isEOS;

    if (RunVenc(vencHandle, &vencInMsg) == -1) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[VideoEncode] RunVenc failed");
        DestroyVenc(vencHandle);
        return HIAI_ERROR;
    }

    DestroyVenc(vencHandle);
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[VideoEncode INFO] process end");

    return HIAI_OK;
}
