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
#include "dvpp_utils.h"
#include "error_code.h"
#include "hiaiengine/c_graph.h"
#include "hiaiengine/log.h"
#include <hiaiengine/status.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct stat Stat;


HIAI_StatusT VDecEngine::Init(const hiai::AIConfig& config, const std::vector<hiai::AIModelDescription>& model_desc)
{
    HIAI_ENGINE_LOG(this, HIAI_OK, "[VDecEngine] init start.");
    if (NULL == pVdecHandle) {
        int ret = CreateVdecApi(pVdecHandle, 0);
        if (0 != ret || NULL == pVdecHandle) {
            HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "pVdecHandle is null.");
            return HIAI_ERROR;
        }
    }
    if (NULL == pDvppHandle) {
        int ret = CreateDvppApi(pDvppHandle);
        if (0 != ret || NULL == pDvppHandle) {
            HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "pDvppHandle is null.");
        }
    }
    vdecInMsg.call_back = VDecEngine::frameCallback;
    HIAI_ENGINE_LOG(this, HIAI_OK, "[VDecEngine] init is finished.");
    return HIAI_OK;
}

VDecEngine::~VDecEngine()
{
    if (NULL != pVdecHandle) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "destroy dvpp api!");
        DestroyVdecApi(pVdecHandle, 0);
        pVdecHandle = NULL;
    }
    if (NULL != pDvppHandle) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "destroy dvpp api!");
        DestroyDvppApi(pDvppHandle);
        pDvppHandle = NULL;
    }
}

HIAI_StatusT VDecEngine::Hfbc2YuvOld(FRAME* frame, vpc_in_msg& vpcInMsg)
{
    if (pDvppHandle != NULL) {
        dvppapi_ctl_msg dvppApiCtlMsg;
        vpcInMsg.format = 0; //YUV420_SEMI_PLANNAR=0
        vpcInMsg.rank = 1; //nv12 =0 ,nv21 =1
        vpcInMsg.bitwidth = frame->bitdepth;
        vpcInMsg.width = frame->width;
        vpcInMsg.high = frame->height;
        // shared_ptr<AutoBuffer> auto_out_buffer = make_shared<AutoBuffer>();
        vpcInMsg.in_buffer = (char*)frame->buffer;
        vpcInMsg.in_buffer_size = frame->buffer_size;
        vpcInMsg.rdma.luma_head_addr = (long)(frame->buffer + frame->offset_head_y);
        vpcInMsg.rdma.chroma_head_addr = (long)(frame->buffer + frame->offset_head_c);
        vpcInMsg.rdma.luma_payload_addr = (long)(frame->buffer + frame->offset_payload_y);
        vpcInMsg.rdma.chroma_payload_addr = (long)(frame->buffer + frame->offset_payload_c);
        vpcInMsg.rdma.luma_head_stride = frame->stride_head;
        vpcInMsg.rdma.chroma_head_stride = frame->stride_head;
        vpcInMsg.rdma.luma_payload_stride = frame->stride_payload;
        vpcInMsg.rdma.chroma_payload_stride = frame->stride_payload;
        vpcInMsg.cvdr_or_rdma = 0;
        vpcInMsg.hmax = frame->realWidth - 1;
        vpcInMsg.hmin = 0;
        vpcInMsg.vmax = frame->realHeight - 1;
        vpcInMsg.vmin = 0;
        vpcInMsg.vinc = 1;
        vpcInMsg.hinc = 1;
        // vpcInMsg.auto_out_buffer_1 = auto_out_buffer;
        uint32_t widthAligned = ALIGN_UP(frame->realWidth, DVPP_STRIDE_WIDTH);
        uint32_t heightAligned = ALIGN_UP(frame->realHeight, DVPP_STRIDE_HEIGHT);
        uint32_t outBufferSize = widthAligned * heightAligned * YUV_BYTES;
        uint8_t* outBuffer = (uint8_t*)HIAI_DVPP_DMalloc(outBufferSize);
        if (NULL == outBuffer) {
            HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[VDecEngine::Hfbc2YuvOld] create outBuffer failed\n");
            return HIAI_ERROR;
        }
        vpcInMsg.out_buffer_1_size = outBufferSize;
        vpcInMsg.out_buffer = (char*)outBuffer;
        dvppApiCtlMsg.in = (void*)(&vpcInMsg);
        dvppApiCtlMsg.in_size = sizeof(vpc_in_msg);
        if (DvppCtl(pDvppHandle, DVPP_CTL_VPC_PROC, &dvppApiCtlMsg) != 0) {
            HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[VDecEngine::Hfbc2YuvOld] call dvppctl fail\n");
            return HIAI_ERROR;
        }
    } else {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[VDecEngine::Hfbc2YuvOld] pDvppHandle is NULL\n");
        return HIAI_ERROR;
    }
    return HIAI_OK;
}

void VDecEngine::frameCallback(FRAME* frame, void* hiai_data)
{

    VDecEngine* vedcPtr = NULL;
    if (hiai_data != NULL) {
        vedcPtr = (VDecEngine*)hiai_data;
    } else {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "hiai_data is NULL");
        return;
    }

    vpc_in_msg vpcInMsg;
    vedcPtr->Hfbc2YuvOld(frame, vpcInMsg);
    std::shared_ptr<DeviceStreamData> out = std::make_shared<DeviceStreamData>();
    out->info = vedcPtr->inputInfo;
    out->info.frameId = vedcPtr->frameId++;
    out->info.isEOS = 0;
    out->imgOrigin.width = frame->realWidth;
    out->imgOrigin.height = frame->realHeight;
    out->imgOrigin.heightAligned = frame->height;
    out->imgOrigin.widthAligned = frame->width;

    out->imgOrigin.buf.data = std::shared_ptr<uint8_t>((uint8_t*)vpcInMsg.out_buffer, HIAI_DVPP_DFree);
    out->imgOrigin.buf.len_of_byte = vpcInMsg.out_buffer_1_size;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &vedcPtr->stamps.second);
    out->info.table["VDecEngine"] = vedcPtr->stamps;
    HIAI_StatusT ret = vedcPtr->SendData(0, "DeviceStreamData", std::static_pointer_cast<void>(out));
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &vedcPtr->stamps.first);
    if (HIAI_OK != ret) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "VDecEngine senddata failed! ret = %d", ret);
        return;
    }
}

HIAI_IMPL_ENGINE_PROCESS("VDecEngine", VDecEngine, VD_INPUT_SIZE)
{
    std::shared_ptr<StreamRawData> inputArg = std::static_pointer_cast<StreamRawData>(arg0);
    // if use hiai
    vdecInMsg.hiai_data = this;
    inputInfo = inputArg->info;
    vdecInMsg.channelId = inputArg->info.channelId;
    if (inputArg->info.isEOS == 1) {
        vdecInMsg.isEOS = 1;
    } else {
        if (inputArg->info.format == H264) {
            memcpy_s(vdecInMsg.video_format, 10, "h264", 4);
        } else {
            memcpy_s(vdecInMsg.video_format, 10, "h265", 4);
        }
        vdecInMsg.in_buffer_size = inputArg->buf.len_of_byte;
        vdecInMsg.in_buffer = (char*)inputArg->buf.data.get();
        vdecInMsg.isEOS = 0;
        //        memcpy_s(vdecInMsg.image_format, 10, "nv12", 4);
    }
    dvppapi_ctl_msg MSG;
    MSG.in_size = sizeof(vdec_in_msg);
    MSG.in = (void*)&(vdecInMsg);
    int ret = VdecCtl(pVdecHandle, DVPP_CTL_VDEC_PROC, &MSG, 0);
    if (0 != ret) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[VDecEngine] DVPP_CTL_VDEC_PROC failed! ret = %d", ret);
        return HIAI_ERROR;
    }

    return HIAI_OK;
}
