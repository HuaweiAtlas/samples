/**
 * ============================================================================
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Atlas Sample
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

#include <stdio.h>
#include <string>
#include "DvppCrop.h"
#include "hiaiengine/log.h"
#include "hiaiengine/c_graph.h"
#include "CropResize.h"

static const float RESIZE_FACTOR_W = 0.5;
static const float RESIZE_FACTOR_H = 0.5;
static const int NUM_ROW_CROP = 1;
static const int NUM_COL_CROP = 1;

void ReleaseHiaiDFreeBuffer(void *ptr)
{
    hiai::HIAIMemory::HIAI_DVPP_DFree(ptr);
}

/**
* @ingroup hiaiengine
* @brief HIAI_DEFINE_PROCESS : implementaion of the engine
* @[in]: engine name and the number of input
 */
HIAI_StatusT DvppCrop::Init(const hiai::AIConfig &config,
                            const std::vector<hiai::AIModelDescription> &model_desc)
{
    return HIAI_OK;
}

HIAI_StatusT DvppCrop::SendDataToDst(uint8_t *&outBuffer, const uint32_t outBufferSize)
{
    // send data to save file engine
    std::shared_ptr<hiai::RawDataBuffer> raw_data_ptr = std::make_shared<hiai::RawDataBuffer>();
    raw_data_ptr->len_of_byte = outBufferSize;
    raw_data_ptr->data.reset(outBuffer, ReleaseHiaiDFreeBuffer);
    HIAI_StatusT ret = HIAI_OK;
    ret = SendData(0, "RawDataBuffer", std::static_pointer_cast<void>(raw_data_ptr));
    if (ret != HIAI_OK) {
        return HIAI_ERROR;
    }

    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "send data success");
    return HIAI_OK;
}

HIAI_IMPL_ENGINE_PROCESS("DvppCrop", DvppCrop, INPUT_SIZE)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[DvppCrop] start process!");
    std::shared_ptr<GraphCtrlInfoT> ctrlInfoT = std::static_pointer_cast<GraphCtrlInfoT>(arg0);
    if (ctrlInfoT == nullptr) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "input arg invaild");
        return HIAI_ERROR;
    }

    shared_ptr<DecodeOutputImage> decodeOutputImage(new DecodeOutputImage);
    shared_ptr<CropResize> cropResize(new CropResize());

    // decode Jpeg
    HIAI_StatusT ret = cropResize->DecodeJpeg(ctrlInfoT->bufferSize, ctrlInfoT->dataBuff, decodeOutputImage);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "DecodeJpeg failed");
        return HIAI_ERROR;
    }

    // calculate the roi area
    vector<CropArea> cropAreaArray = cropResize->getMulCropArea((uint32_t)decodeOutputImage->imgWidth *
                                                                RESIZE_FACTOR_W,
                                                                (uint32_t)decodeOutputImage->imgHeight * RESIZE_FACTOR_H, NUM_ROW_CROP, NUM_COL_CROP);
    if (cropAreaArray.size() <= 0) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Get roi area failed.");
        return HIAI_ERROR;
    }

    // calculate the output size
    uint32_t outBufferSize = cropResize->getYuvOutputBufferSize(decodeOutputImage, RESIZE_FACTOR_W, RESIZE_FACTOR_H);

    uint8_t *outBuffer = (uint8_t *)HIAI_DVPP_DMalloc(outBufferSize);

    CropResizePara cropResizePara;
    for (int i = 0; i < cropAreaArray.size(); i++) {
        cropResizePara.cropAreaArray.push_back(cropAreaArray[i]);
    }
    cropResizePara.resizeFactorW = RESIZE_FACTOR_W;
    cropResizePara.resizeFactorH = RESIZE_FACTOR_H;
    cropResizePara.inputFormat = INPUT_YUV420_SEMI_PLANNER_VU;
    cropResizePara.outputFormat = OUTPUT_YUV420SP_VU;

    // crop and resize
    ret = cropResize->CropResizeImage(decodeOutputImage, cropResizePara, outBuffer, outBufferSize);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Crop resize image failed");
        HIAI_DVPP_DFree(outBuffer);
        return HIAI_ERROR;
    }

    ret = SendDataToDst(outBuffer, outBufferSize);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Send data to next engine falide.");
        return HIAI_ERROR;
    }

    return HIAI_OK;
}

DvppCrop::~DvppCrop()
{
}