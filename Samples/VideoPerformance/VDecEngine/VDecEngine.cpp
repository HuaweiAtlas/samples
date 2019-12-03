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
#include "VDecEngine.h"
#include "error_code.h"
#include "hiaiengine/c_graph.h"
#include "hiaiengine/log.h"
#include <hiaiengine/status.h>
#include <sys/stat.h>
#include <unistd.h>
#include "hiaiengine/ai_memory.h"

static const uint32_t NUM_TWO = 2;
static const uint32_t NUM_THREE = 3;
static const uint32_t DEST_MAX = 10;

uint8_t g_channelId = 0;

HIAI_REGISTER_DATA_TYPE("ResultInfoT", ResultInfoT);

HIAI_StatusT VDecEngine::Init(const hiai::AIConfig& config, const std::vector<hiai::AIModelDescription>& model_desc)
{
    HIAI_ENGINE_LOG(this, HIAI_IDE_ERROR, "[VDecEngine] init start.");
    if (pVdecHandle == nullptr) {
        int ret = CreateVdecApi(pVdecHandle, 0);
        if (ret != 0 || pVdecHandle == nullptr) {
            HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "pVdecHandle is null.");
            return HIAI_ERROR;
        }
    }
    if (pDvppHandle == nullptr) {
        int ret = CreateDvppApi(pDvppHandle);
        if (ret != 0 || pDvppHandle == nullptr) {
            HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "pDvppHandle is null.");
        }
    }
    HIAI_ENGINE_LOG(this, HIAI_OK, "[VDecEngine] init is finished.");
    return HIAI_OK;
}

VDecEngine::~VDecEngine()
{
    if (pVdecHandle != nullptr) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "destroy dvpp api!");
        DestroyVdecApi(pVdecHandle, 0);
        pVdecHandle = nullptr;
    }
    if (pDvppHandle != nullptr) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "destroy dvpp api!");
        DestroyDvppApi(pDvppHandle);
        pDvppHandle = nullptr;
    }
}

void VDecEngine::ConstructVpcData(FRAME* frame, uint8_t* outputBuffer, 
    std::shared_ptr<VpcUserImageConfigure>& userImage, std::shared_ptr<VpcUserRoiConfigure>& roiConfigure)
{
    userImage->bareDataAddr = nullptr;
    userImage->bareDataBufferSize = 0;
    userImage->widthStride = frame->width;
    userImage->heightStride = frame->height;
    string imageFormat(frame->image_format);
    userImage->inputFormat = INPUT_YUV420_SEMI_PLANNER_VU;
    userImage->outputFormat = OUTPUT_YUV420SP_UV;
    userImage->isCompressData = true;
    VpcCompressDataConfigure* compressDataConfigure = &userImage->compressDataConfigure;
    uintptr_t baseAddr = (uintptr_t)frame->buffer;
    compressDataConfigure->lumaHeadAddr = baseAddr + frame->offset_head_y;
    compressDataConfigure->chromaHeadAddr = baseAddr + frame->offset_head_c;
    compressDataConfigure->lumaPayloadAddr = baseAddr + frame->offset_payload_y;
    compressDataConfigure->chromaPayloadAddr = baseAddr + frame->offset_payload_c;
    compressDataConfigure->lumaHeadStride = frame->stride_head;
    compressDataConfigure->chromaHeadStride = frame->stride_head;
    compressDataConfigure->lumaPayloadStride = frame->stride_payload;
    compressDataConfigure->chromaPayloadStride = frame->stride_payload;
    userImage->yuvSumEnable = false;
    userImage->cmdListBufferAddr = nullptr;
    userImage->cmdListBufferSize = 0;
    roiConfigure->next = nullptr;
    userImage->roiConfigure = roiConfigure.get();
    VpcUserRoiInputConfigure* roiInput = &roiConfigure->inputConfigure;
    roiInput->cropArea.leftOffset = 0;
    roiInput->cropArea.rightOffset = (frame->width % NUM_TWO) ? frame->width : (frame->width - 1);
    roiInput->cropArea.upOffset = 0;
    roiInput->cropArea.downOffset = (frame->height % NUM_TWO) ? frame->height : (frame->height - 1);
    VpcUserRoiOutputConfigure* roiOutput = &roiConfigure->outputConfigure;
    roiOutput->outputArea.leftOffset = 0;
    roiOutput->outputArea.rightOffset = (frame->width % NUM_TWO) ? frame->width : (frame->width - 1);
    roiOutput->outputArea.upOffset = 0;
    roiOutput->outputArea.downOffset = (frame->height % NUM_TWO) ? frame->height : (frame->height - 1);
    roiOutput->widthStride = ALIGN_UP(frame->width, DVPP_STRIDE_WIDTH);
    roiOutput->heightStride = ALIGN_UP(frame->height, DVPP_STRIDE_HEIGHT);
    roiOutput->bufferSize = roiOutput->widthStride * roiOutput->heightStride * NUM_THREE / NUM_TWO;
    roiOutput->addr = outputBuffer;
    roiInput->cropArea = roiConfigure->inputConfigure.cropArea;
}

HIAI_StatusT VDecEngine::Hfbc2YuvNew(FRAME *frame,  uint8_t *outputBuffer)
{
    if (pDvppHandle == nullptr) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[VDecEngine::Hfbc2YuvNew] pDvppHandle is NULL\n");
        return HIAI_ERROR;
    }
    std::shared_ptr<VpcUserImageConfigure> userImage(new VpcUserImageConfigure);
    std::shared_ptr<VpcUserRoiConfigure> roiConfigure(new VpcUserRoiConfigure);
    ConstructVpcData(frame, outputBuffer, userImage, roiConfigure);
    dvppapi_ctl_msg dvppApiCtlMsg;
    dvppApiCtlMsg.in = static_cast<void *>(userImage.get());
    dvppApiCtlMsg.in_size = sizeof(VpcUserImageConfigure);
    int ret = DvppCtl(pDvppHandle, DVPP_CTL_VPC_PROC, &dvppApiCtlMsg);
    if (ret != 0) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[VDecEngine::Hfbc2YuvNew] call dvppctl fail\n");
        DestroyDvppApi(pDvppHandle);
        return HIAI_ERROR;
    }
    return HIAI_OK;
}

void VDecEngine::frameCallback(FRAME* frame, void* hiai_data)
{

    VDecEngine* vdecPtr = nullptr;
    if (hiai_data != nullptr) {
        vdecPtr = (VDecEngine*)hiai_data;
    } else {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "hiai_data is NULL");
        return;
    }
    uint8_t *outputBuffer = nullptr;
    uint32_t widthStride = ALIGN_UP(frame->width, DVPP_STRIDE_WIDTH);
    uint32_t heightStride = ALIGN_UP(frame->height, DVPP_STRIDE_HEIGHT);
    uint32_t bufferSize = widthStride * heightStride * NUM_THREE / NUM_TWO;

    HIAI_StatusT ret = hiai::HIAIMemory::HIAI_DVPP_DMalloc(bufferSize, (void *&)outputBuffer);

    // call vpc interface to decompress hfbc
    vdecPtr->Hfbc2YuvNew(frame, outputBuffer);
    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Vdec success!");
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_ERROR, "Vdec failed");
    }
    g_mt.lock();
    g_decodeCount--;
    if (g_decodeCount == 0) {
        gettimeofday(&(g_resultInfo->endTime), NULL);
        ret = vdecPtr->SendData(0, "ResultInfoT", std::static_pointer_cast<void>(g_resultInfo));
        if (ret != HIAI_OK) {
            HIAI_ENGINE_LOG(HIAI_ERROR, "SendData failed! ret = %d", ret);
        }
    }
    g_mt.unlock();
    hiai::HIAIMemory::HIAI_DVPP_DFree(outputBuffer);
}

HIAI_IMPL_ENGINE_PROCESS("VDecEngine", VDecEngine, VD_INPUT_SIZE)
{
    std::shared_ptr<GraphCtrlInfo> inputArg = std::static_pointer_cast<GraphCtrlInfo>(arg0);
    vdec_in_msg vdecInMsg;
    vdecInMsg.hiai_data = this;
    vdecInMsg.channelId = g_channelId;
    vdecInMsg.call_back = VDecEngine::frameCallback;
    g_channelId++;
    std::string formatFlag;
    if (inputArg->testType == 0) {
        formatFlag = "h264";
    } else if (inputArg->testType == 1) {
        formatFlag = "h265";
    }
    errno_t res =  memcpy_s(vdecInMsg.video_format, DEST_MAX, formatFlag.c_str(), formatFlag.size());
    if (res != EOK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[VideoEncode] memcpy_s() return error");
        return HIAI_ERROR;
    }
    vdecInMsg.in_buffer_size = inputArg->bufferSize;
    vdecInMsg.in_buffer = (char*)inputArg->dataBuff.get();
    vdecInMsg.isEOS = 0;

    dvppapi_ctl_msg MSG;
    MSG.in_size = sizeof(vdec_in_msg);
    MSG.in = (void*)&(vdecInMsg);
    if (g_decodeCount == 0) {
        g_decodeCount = inputArg->decodeCount;
        gettimeofday(&(g_resultInfo->startTime), NULL);
    }
    int ret = VdecCtl(pVdecHandle, DVPP_CTL_VDEC_PROC, &MSG, 0);
    if (0 != ret) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[VDecEngine] DVPP_CTL_VDEC_PROC failed! ret = %d", ret);
        return HIAI_ERROR;
    }

    return HIAI_OK;
}
