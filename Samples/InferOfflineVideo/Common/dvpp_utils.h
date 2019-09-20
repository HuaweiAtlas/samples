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
#ifndef ATLASFACEDEMO_DVPP_UTILS_H
#define ATLASFACEDEMO_DVPP_UTILS_H

#include "common_data_type.h"
#include "dvpp/Vpc.h"
#include "dvpp/idvppapi.h"
#include "error_code.h"

#define DVPP_STRIDE_WIDTH 128
#define DVPP_STRIDE_HEIGHT 16

#define DVPP_STRIDE_128 128
#define DVPP_STRIDE_16 16
#define DVPP_STRIDE_2 2

#define CHECK_ODD(NUM) (((NUM) % 2 != 0) ? (NUM) : (NUM - 1))
#define CHECK_EVEN(NUM) (((NUM) % 2 == 0) ? (NUM) : (NUM - 1))

static HIAI_StatusT vpcResize(uint8_t* inBuffer, uint32_t inWidth, uint32_t inHeight,
    uint8_t* outBuffer, uint32_t outWidth, uint32_t outHeight,
    VpcInputFormat inputFormat = INPUT_YUV420_SEMI_PLANNER_UV)
{
    int32_t ret = 0;
    uint32_t inWidthStride = ALIGN_UP(inWidth, DVPP_STRIDE_WIDTH);
    uint32_t inHeightStride = ALIGN_UP(inHeight, DVPP_STRIDE_HEIGHT);
    uint32_t outWidthStride = ALIGN_UP(outWidth, DVPP_STRIDE_WIDTH);
    uint32_t outHeightStride = ALIGN_UP(outHeight, DVPP_STRIDE_HEIGHT);
//    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "inDataBuffer inWidth %d inHeight %d inWidthStride %d inHeightStride %d\n",
//        inWidth, inHeight, inWidthStride, inHeightStride);
//    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "outDataBuffer outWidth %d outHeight %d outWidthStride %d outHeight %d\n",
//        outWidth, outHeight, outWidthStride, outHeightStride);
    // inBuffer yuv420sp_nv21_vu图像
    uint32_t inBufferSize = inWidthStride * inHeightStride * YUV_BYTES;
    // outBuffer yuv420sp_nv12_uv图像
    uint32_t outBufferSize = outWidthStride * outHeightStride * YUV_BYTES;
    // 构造输入图片配置
    std::shared_ptr<VpcUserImageConfigure> imageConfigure(new VpcUserImageConfigure);
    imageConfigure->bareDataAddr = inBuffer;
    imageConfigure->bareDataBufferSize = inBufferSize;
    imageConfigure->isCompressData = false;
    imageConfigure->widthStride = inWidthStride;
    imageConfigure->heightStride = inHeightStride;
    imageConfigure->inputFormat = inputFormat;
    imageConfigure->outputFormat = OUTPUT_YUV420SP_UV;
    imageConfigure->yuvSumEnable = false;
    imageConfigure->cmdListBufferAddr = nullptr;
    imageConfigure->cmdListBufferSize = 0;
    std::shared_ptr<VpcUserRoiConfigure> roiConfigure(new VpcUserRoiConfigure);
    roiConfigure->next = nullptr;
    // 设置抠图区域，抠图区域左上角坐标[0,0],右下角坐标[1919,1079]
    VpcUserRoiInputConfigure* inputConfigure = &roiConfigure->inputConfigure;
    inputConfigure->cropArea.leftOffset = 0;
    inputConfigure->cropArea.rightOffset = CHECK_ODD(inWidth - 1);
    inputConfigure->cropArea.upOffset = 0;
    inputConfigure->cropArea.downOffset = CHECK_ODD(inHeight - 1);
    VpcUserRoiOutputConfigure* outputConfigure = &roiConfigure->outputConfigure;
    outputConfigure->addr = outBuffer;
    outputConfigure->bufferSize = outBufferSize;
    outputConfigure->widthStride = outWidthStride;
    outputConfigure->heightStride = outHeightStride;
    // 设置贴图区域，贴图区域左上角坐标[0,0],右下角坐标[1279,719]
    outputConfigure->outputArea.leftOffset = 0;
    outputConfigure->outputArea.rightOffset = CHECK_ODD(outWidth - 1);
    outputConfigure->outputArea.upOffset = 0;
    outputConfigure->outputArea.downOffset = CHECK_ODD(outHeight - 1);
    imageConfigure->roiConfigure = roiConfigure.get();
    IDVPPAPI* pidvppapi = nullptr;
    ret = CreateDvppApi(pidvppapi);
    if (0 != ret) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "create dvpp api fail.");
        return HIAI_ERROR;
    }
    dvppapi_ctl_msg dvppApiCtlMsg;
    dvppApiCtlMsg.in = static_cast<void*>(imageConfigure.get());
    dvppApiCtlMsg.in_size = sizeof(VpcUserImageConfigure);
    ret = DvppCtl(pidvppapi, DVPP_CTL_VPC_PROC, &dvppApiCtlMsg);
    if (0 != ret) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "call vpc dvppctl process faild!");
        ret = DestroyDvppApi(pidvppapi);
        return HIAI_ERROR;
    }
    ret = DestroyDvppApi(pidvppapi);
    if (0 != ret) {
        return HIAI_ERROR;
    }
    return HIAI_OK;
}

static HIAI_StatusT vpcCropResize(uint8_t* inBuffer, uint32_t inWidth, uint32_t inHeight,
    uint8_t* outBuffer, uint32_t outWidth, uint32_t outHeight,
    const std::vector<DetectInfo>& rects,
    VpcInputFormat inputFormat = INPUT_YUV420_SEMI_PLANNER_UV)
{
    int ret = 0;
    uint32_t inWidthStride = ALIGN_UP(inWidth, DVPP_STRIDE_WIDTH);
    uint32_t inHeightStride = ALIGN_UP(inHeight, DVPP_STRIDE_HEIGHT);
    uint32_t outWidthStride = ALIGN_UP(outWidth, DVPP_STRIDE_WIDTH);
    uint32_t outHeightStride = ALIGN_UP(outHeight, DVPP_STRIDE_HEIGHT);
    uint32_t inBufferSize = inWidthStride * inHeightStride * YUV_BYTES; //  yuv420sp图像
    uint32_t outPatchSize = outWidthStride * outHeightStride * YUV_BYTES; //  yuv420sp图像

    // 构造输入图片配置
    std::shared_ptr<VpcUserImageConfigure> imageConfigure(new VpcUserImageConfigure);
    imageConfigure->bareDataAddr = inBuffer;
    imageConfigure->bareDataBufferSize = inBufferSize;
    imageConfigure->isCompressData = false;
    imageConfigure->widthStride = inWidthStride;
    imageConfigure->heightStride = inHeightStride;
    imageConfigure->inputFormat = inputFormat;
    imageConfigure->outputFormat = OUTPUT_YUV420SP_UV;
    imageConfigure->yuvSumEnable = false;
    imageConfigure->cmdListBufferAddr = nullptr;
    imageConfigure->cmdListBufferSize = 0;
    VpcUserRoiConfigure* lastRoi;
    std::vector<VpcUserRoiConfigure> roiConfigures(rects.size());
    uint8_t* dataPtr = outBuffer;
    for (int i = 0; i < rects.size(); i++) {
        VpcUserRoiConfigure* roiConfigure = &roiConfigures[i];
        roiConfigure->next = nullptr;
        VpcUserRoiInputConfigure* inputConfigure = &roiConfigure->inputConfigure;
        // 设置抠图区域
        inputConfigure->cropArea.leftOffset = CHECK_EVEN(int(rects[i].location.anchor_lt.x));
        inputConfigure->cropArea.rightOffset = CHECK_ODD(int(rects[i].location.anchor_rb.x));
        inputConfigure->cropArea.upOffset = CHECK_EVEN(int(rects[i].location.anchor_lt.y));
        inputConfigure->cropArea.downOffset = CHECK_ODD(int(rects[i].location.anchor_rb.y));
        VpcUserRoiOutputConfigure* outputConfigure = &roiConfigure->outputConfigure;
        outputConfigure->addr = dataPtr;
        outputConfigure->bufferSize = outPatchSize;
        outputConfigure->widthStride = outWidthStride;
        outputConfigure->heightStride = outHeightStride;
        // 设置贴图区域
        outputConfigure->outputArea.leftOffset = 0; // 这个偏移值需要16对齐
        outputConfigure->outputArea.rightOffset = CHECK_ODD(outWidth - 1);
        outputConfigure->outputArea.upOffset = 0;
        outputConfigure->outputArea.downOffset = CHECK_ODD(outHeight - 1);
        if (i == 0) {
            imageConfigure->roiConfigure = roiConfigure;
            lastRoi = roiConfigure;
        } else {
            lastRoi->next = roiConfigure;
            lastRoi = roiConfigure;
        }
        dataPtr += outPatchSize;
    }
    IDVPPAPI* pidvppapi = nullptr;
    ret = CreateDvppApi(pidvppapi);
    if (ret != 0) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[vpcCrop] create dvpp api fail.");
        return HIAI_ERROR;
    }
    dvppapi_ctl_msg dvppApiCtlMsg;
    dvppApiCtlMsg.in = static_cast<void*>(imageConfigure.get());
    dvppApiCtlMsg.in_size = sizeof(VpcUserImageConfigure);
    ret = DvppCtl(pidvppapi, DVPP_CTL_VPC_PROC, &dvppApiCtlMsg);
    if (0 != ret) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[vpcCrop] call vpc dvppctl process faild!\n");
        ret = DestroyDvppApi(pidvppapi);
        return HIAI_ERROR;
    }
    ret = DestroyDvppApi(pidvppapi);
    return HIAI_OK;
}

#endif //ATLASFACEDEMO_DVPP_UTILS_H
