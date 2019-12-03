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
#include <fstream>
#include <memory>
#include <sstream>
#include <cstdio>
#include <cstring>
#include "JpegDecode.h"
#include "hiaiengine/ai_memory.h"
#include "hiaiengine/c_graph.h"
#include "hiaiengine/data_type_reg.h"
#include "hiaiengine/log.h"
#include "CropResize.h"

// register EngineImageTransT
HIAI_REGISTER_SERIALIZE_FUNC("EngineImageTransT", EngineImageTransT, GetEngineImageTransPtr, GetEngineImageTransrPtr);
/**
* @ingroup hiaiengine
* @brief HIAI_DEFINE_PROCESS : implementaion of the engine
* @[in]: engine name and the number of input
 */
HIAI_StatusT JpegDecode::Init(const hiai::AIConfig &config,
                              const std::vector<hiai::AIModelDescription> &model_desc)
{
    return HIAI_OK;
}

HIAI_StatusT JpegDecode::SendDataToDst(const shared_ptr<CropResizeOutputImage> cropResizeOutputImage)
{
    std::shared_ptr<DeviceStreamData> deviceStreamData = std::make_shared<DeviceStreamData>();
    deviceStreamData->imgOrigin.width = cropResizeOutputImage->imgWidth;
    deviceStreamData->imgOrigin.height = cropResizeOutputImage->imgHeight;
    deviceStreamData->imgOrigin.widthAligned = cropResizeOutputImage->imgWidthAligned;
    deviceStreamData->imgOrigin.heightAligned = cropResizeOutputImage->imgHeightAligned;

    deviceStreamData->imgOrigin.buf.data = std::shared_ptr<uint8_t>((uint8_t*)cropResizeOutputImage->outBuffer, HIAI_DVPP_DFree);
    deviceStreamData->imgOrigin.buf.len_of_byte = cropResizeOutputImage->outBufferSize;

    // put the whole image into classification
    const uint32_t imgWidth = deviceStreamData->imgOrigin.width;
    const uint32_t imgHeight = deviceStreamData->imgOrigin.height;
    DetectInfo info;
    info.location.anchor_lt.x = 0.f;
    info.location.anchor_lt.y = 0.f;
    info.location.anchor_rb.x = imgWidth - 1;
    info.location.anchor_rb.y = imgHeight - 1;
    info.confidence = 1;
    deviceStreamData->detectResult.push_back(info);

    HIAI_StatusT ret = SendData(0, "DeviceStreamData", std::static_pointer_cast<void>(deviceStreamData));

    if (ret != HIAI_OK) {
        return HIAI_ERROR;
    }

    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "send data success");
    return HIAI_OK;
}

HIAI_IMPL_ENGINE_PROCESS("JpegDecode", JpegDecode, JPEGD_RESIZE_INPUT_SIZE)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[JPEGDResize] start process!");
    std::shared_ptr<EngineImageTransT> ctrlInfoT = std::static_pointer_cast<EngineImageTransT>(arg0);
    if (ctrlInfoT == nullptr) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[JPEGDResize] input arg invaild");
        return HIAI_ERROR;
    }

    HIAI_StatusT ret = HIAI_ERROR;
    shared_ptr<DecodeOutputImage> decodeOutputImage(new DecodeOutputImage);
    shared_ptr<CropResize> cropResize(new CropResize());

    // decode Jpeg
    ret = cropResize->DecodeJpeg(ctrlInfoT->buffer_size, ctrlInfoT->trans_buff, decodeOutputImage);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[JPEGDResize] DecodeJpeg failed");
        return HIAI_ERROR;
    }

    float resizeFactorW = 1;
    float resizeFactorH = 1;

    vector<CropArea> cropAreaArray;
    CropArea cropArea;
    cropArea.cropLeftOffset = 0;
    cropArea.cropRightOffset = CHECK_ODD((uint32_t)decodeOutputImage->imgWidth - 1);
    cropArea.cropUpOffset = 0;
    cropArea.cropDownOffset = CHECK_ODD((uint32_t)decodeOutputImage->imgHeight - 1);

    cropArea.outputLeftOffset = 0;
    cropArea.outputRightOffset = CHECK_ODD((uint32_t)decodeOutputImage->imgWidth - 1);
    cropArea.outputUpOffset = 0;
    cropArea.outputDownOffset = CHECK_ODD((uint32_t)decodeOutputImage->imgHeight - 1);
    cropAreaArray.push_back(cropArea);

    uint32_t outBufferSize = cropResize->GetYuvOutputBufferSize(decodeOutputImage, resizeFactorW, resizeFactorH);

    uint8_t *outBuffer = nullptr;
    ret = hiai::HIAIMemory::HIAI_DVPP_DMalloc(outBufferSize, (void *&)outBuffer);

    CropResizePara cropResizePara;
    for (int i = 0; i < cropAreaArray.size(); i++) {
        cropResizePara.cropAreaArray.push_back(cropAreaArray[i]);
    }
    cropResizePara.resizeFactorW = resizeFactorW;
    cropResizePara.resizeFactorH = resizeFactorH;
    cropResizePara.inputFormat = INPUT_YUV420_SEMI_PLANNER_VU;
    cropResizePara.outputFormat = OUTPUT_YUV420SP_UV;

    shared_ptr<CropResizeOutputImage> cropResizeOutputImage(new CropResizeOutputImage);
    cropResizeOutputImage->outBufferSize = outBufferSize;
    cropResizeOutputImage->outBuffer = outBuffer;
    // crop and resize
    ret = cropResize->CropResizeImage(decodeOutputImage, cropResizePara, cropResizeOutputImage);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[JPEGDResize] Resize image failed");
        HIAI_DVPP_DFree(outBuffer);
        return HIAI_ERROR;
    }
    ret = SendDataToDst(cropResizeOutputImage);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[JPEGDResize] Send data to next engine falide.");
        return HIAI_ERROR;
    }

    return HIAI_OK;
}
JpegDecode::~JpegDecode()
{
}
