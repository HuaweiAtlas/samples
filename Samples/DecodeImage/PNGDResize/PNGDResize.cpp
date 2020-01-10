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
#include "PNGDResize.h"
#include "hiaiengine/ai_memory.h"
#include "hiaiengine/c_graph.h"
#include "hiaiengine/data_type_reg.h"
#include "hiaiengine/log.h"
#include <fstream>
#include <memory>
#include <sstream>
#include <cstdio>
#include <string>

/**
* @ingroup hiaiengine
* @brief HIAI_DEFINE_PROCESS : implementaion of the engine
* @[in]: engine name and the number of input
 */
HIAI_StatusT PNGDResize::Init(const hiai::AIConfig &config,
                              const std::vector<hiai::AIModelDescription> &model_desc)
{
    return HIAI_OK;
}

HIAI_StatusT PNGDResize::ResizeImage(const std::shared_ptr<DecodeOutputImage> decodeOutputImage,
                                     shared_ptr<CropResizeOutputImage> cropResizeOutputImage,
                                     const float resizeFactorW, const float resizeFactorH)
{
    // Dvpp的缩放与抠图都是调用VPC接口，无论是缩放和抠图，都要设置抠图和贴图区域
    // 除8K缩放功能外，抠图区域和贴图区域最小分辨率为10*6，最大分辨率为4096*4096
    // 8K缩放，最大分辨率支持4096*4096~8192*8192，格式仅支持yuv420，输出分辨率支持16*16~4096*4096
    // CropArea 封装抠图和贴图区域参数的结构体
    vector<CropArea> cropAreaArray;
    // 抠图区域：对原图像进行操作的区域，区域必须在原图内。decodeOutputImage为解码输出，即为VPC原图，
    // 本例子主要演示缩放的过程，以下为设置了整张原图为抠图区域，实际上抠图区域可以灵活变化，但必须在原图内
    // 抠图参数cropLeftOffset(左偏移) cropRightOffset(右偏移) cropUpOffset(上偏移) cropDownOffset(下偏移)
    CropArea cropArea;
    cropArea.cropLeftOffset = 0;
    cropArea.cropRightOffset = CHECK_ODD((uint32_t)decodeOutputImage->imgWidth - 1);
    cropArea.cropUpOffset = 0;
    cropArea.cropDownOffset = CHECK_ODD((uint32_t)decodeOutputImage->imgHeight - 1);

    // 贴图区域：对抠图区域进行缩放后，贴在底图的位置，其大小与抠图区域和缩放系数有关，位置不能超过缩放后图片的范围
    // 以下贴图区域设置为缩放后图片
    // 抠图参数outputLeftOffset(左偏移) outputRightOffset(右偏移) outputUpOffset(上偏移) outputDownOffset(下偏移)
    cropArea.outputLeftOffset = 0;
    cropArea.outputRightOffset = CHECK_ODD((uint32_t)(decodeOutputImage->imgWidth * resizeFactorW) - 1);
    cropArea.outputUpOffset = 0;
    cropArea.outputDownOffset = CHECK_ODD((uint32_t)(decodeOutputImage->imgHeight * resizeFactorH) - 1);
    cropAreaArray.push_back(cropArea);

    // 计算缩放后的大小
    shared_ptr<CropResize> cropResize(new CropResize());
    uint32_t outBufferSize = cropResize->GetYuvOutputBufferSize(decodeOutputImage, resizeFactorW, resizeFactorH);
    // 用HIAI_DVPP_DMalloc申请内存
    uint8_t *outBuffer = (uint8_t *)HIAI_DVPP_DMalloc(outBufferSize);

    CropResizePara cropResizePara;
    for (int i = 0; i < cropAreaArray.size(); i++) {
        cropResizePara.cropAreaArray.push_back(cropAreaArray[i]);
    }
    // 宽高的缩放系数与上文抠图缩放系数应保持一直，避免出错
    cropResizePara.resizeFactorW = resizeFactorW;
    cropResizePara.resizeFactorH = resizeFactorH;
    cropResizePara.inputFormat = INPUT_RGB;
    cropResizePara.outputFormat = OUTPUT_YUV420SP_VU;

    cropResizeOutputImage->outBufferSize = outBufferSize;
    cropResizeOutputImage->outBuffer = outBuffer;
    // 调用封装好的方法进行缩放
    HIAI_StatusT ret = cropResize->CropResizeImage(decodeOutputImage, cropResizePara, cropResizeOutputImage);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[JPEGDResize] Resize image failed");
        HIAI_DVPP_DFree(outBuffer);
        return HIAI_ERROR;
    }

    return HIAI_OK;
}

HIAI_StatusT PNGDResize::SendDataToDst(const shared_ptr<CropResizeOutputImage> cropResizeOutputImage)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "begin send\n");
    uint32_t bufferLenExtend = 0;
    std::shared_ptr<EngineImageTransT> output =
        std::make_shared<EngineImageTransT>();
    output->trans_buff = std::shared_ptr<uint8_t>((uint8_t *)cropResizeOutputImage->outBuffer, HIAI_DVPP_DFree);
    ;
    output->buffer_size = cropResizeOutputImage->outBufferSize;
    output->width = cropResizeOutputImage->imgWidthAligned;
    output->height = cropResizeOutputImage->imgHeightAligned;
    output->trans_buff_extend.reset(cropResizeOutputImage->outBuffer + cropResizeOutputImage->outBufferSize, [](uint8_t *p) {});
    output->buffer_size_extend = bufferLenExtend;

    HIAI_StatusT ret = SendData(0, "EngineImageTransT", std::static_pointer_cast<EngineImageTransT>(output));
    if (ret != HIAI_OK) {
        return HIAI_ERROR;
    }

    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "send data success");
    return HIAI_OK;
}

HIAI_IMPL_ENGINE_PROCESS("PNGDResize", PNGDResize, PNGD_INPUT_SIZE)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[PNGDResize] start process!");
    std::shared_ptr<EngineImageTransT> ctrlInfoT = std::static_pointer_cast<EngineImageTransT>(arg0);
    if (ctrlInfoT == nullptr) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[PNGDResize] input arg invaild");
        return HIAI_ERROR;
    }

    shared_ptr<DecodeOutputImage> decodeOutputImage(new DecodeOutputImage);
    shared_ptr<CropResize> cropResize(new CropResize());

    // decode Png
    HIAI_StatusT ret = cropResize->DecodePng(ctrlInfoT->buffer_size, ctrlInfoT->trans_buff, decodeOutputImage);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[PNGDResize] Decode Png failed");
        return HIAI_ERROR;
    }

    // crop and resize
    // 宽高的缩放系数范围为[1/32, 16]
    // 除8K缩放功能外，[10*6，4096*4096] 8K缩放仅支持yuv420, 最大分辨率4096*4096~8192*8192,
    // 输出分辨率支持16*16~4096*4096
    // 用户在设置宽高缩放系数的时候，除了考虑[1/32, 16]，还应考虑输出输出分辨率的限制
    float resizeFactorW = 1;
    float resizeFactorH = 1;

    shared_ptr<CropResizeOutputImage> cropResizeOutputImage(new CropResizeOutputImage);
    ret = ResizeImage(decodeOutputImage, cropResizeOutputImage, resizeFactorW, resizeFactorH);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[JPEGDResize] Resize image failed");
        return HIAI_ERROR;
    }

    cropResize->CbFreePng();
    // send data to next engine
    ret = SendDataToDst(cropResizeOutputImage);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[JPEGDResize] Send data to next engine falide.");
        return HIAI_ERROR;
    }

    return HIAI_OK;
}

PNGDResize::~PNGDResize()
{
}
