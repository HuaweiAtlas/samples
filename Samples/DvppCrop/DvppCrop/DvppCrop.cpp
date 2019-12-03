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

static const uint32_t CROP_NUM_COLROW = 2;

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

HIAI_StatusT DvppCrop::SendDataToDst(const shared_ptr<CropResizeOutputImage> cropResizeOutputImage)
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
    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "send data success");
    return HIAI_OK;
}

HIAI_StatusT DvppCrop::CropImage(const std::shared_ptr<DecodeOutputImage> decodeOutputImage,
                                 shared_ptr<CropResizeOutputImage> cropResizeOutputImage,
                                 const float resizeFactorW, const float resizeFactorH)
{
    // Dvpp的缩放与抠图都是调用VPC接口，无论是缩放和抠图，都要设置抠图和贴图区域
    // 除8K缩放功能外，抠图区域和贴图区域最小分辨率为10*6，最大分辨率为4096*4096
    // 8K缩放，最大分辨率支持4096*4096~8192*8192，格式仅支持yuv420，输出分辨率支持16*16~4096*4096
    // CropArea 封装抠图和贴图区域参数的结构体
    vector<CropArea> cropAreaArray;
    uint32_t blockWidth = (uint32_t)(decodeOutputImage->imgWidth * resizeFactorW) / CROP_NUM_COLROW;
    uint32_t blockHeigth = (uint32_t)(decodeOutputImage->imgHeight * resizeFactorH) / CROP_NUM_COLROW;
    uint32_t baseWidth = CHECK_ODD((uint32_t)(decodeOutputImage->imgWidth * resizeFactorW) - 1);
    uint32_t baseHeight = CHECK_ODD((uint32_t)(decodeOutputImage->imgHeight * resizeFactorH) - 1);
    // 以下for循环是设置4个抠图参数，将抠图参数cropArea push到cropAreaArray，可以一次性将扣4张图，输出到底图
    // 用户可以根据需求自由设置抠图和贴图区域，一次性抠图贴图的最大个数为256
    for (int i = 0; i < CROP_NUM_COLROW; i++) {
        for (int j = 0; j < CROP_NUM_COLROW; j++) {
            CropArea cropArea;
            // 以下为设置了整张原图中心的一部分为抠图区域，实际上抠图区域可以灵活变化，但必须在原图内
            // 抠图区域：对原图像进行操作的区域，区域必须在原图内。decodeOutputImage为解码输出，即为VPC原图，
            // 抠图参数cropLeftOffset(左偏移) cropRightOffset(右偏移) cropUpOffset(上偏移) cropDownOffset(下偏移)
            cropArea.cropLeftOffset = CHECK_EVEN(blockWidth / CROP_NUM_COLROW);
            cropArea.cropRightOffset = CHECK_ODD((uint32_t)decodeOutputImage->imgWidth - blockWidth / CROP_NUM_COLROW - 1);
            cropArea.cropUpOffset = CHECK_EVEN(blockHeigth / CROP_NUM_COLROW);
            cropArea.cropDownOffset = CHECK_ODD((uint32_t)decodeOutputImage->imgHeight - blockHeigth / CROP_NUM_COLROW - 1);

            // 贴图区域：对抠图区域贴在底图四个角的位置, 用户可以自由设置，位置不能超过输出底图的范围
            // 以下贴图区域设置为缩放后图片
            // 抠图参数outputLeftOffset(左偏移) outputRightOffset(右偏移) outputUpOffset(上偏移) outputDownOffset(下偏移)
            cropArea.outputLeftOffset = ALIGN_UP(i * blockWidth, WIDTH_ALIGNED);  // 必须为16对其
            // 防止贴图范围超过底图
            uint32_t tmpRightOffset = cropArea.outputLeftOffset + blockWidth - 1;
            uint32_t mapRightOffset = tmpRightOffset < baseWidth ? tmpRightOffset : baseWidth;
            cropArea.outputRightOffset = CHECK_ODD(mapRightOffset);

            cropArea.outputUpOffset = CHECK_EVEN(j * blockHeigth);
            // 防止贴图范围超过底图
            uint32_t tmpDownOffset = cropArea.outputUpOffset + blockHeigth - 1;
            uint32_t mapDownOffset = tmpDownOffset < baseHeight ? tmpDownOffset : baseHeight;
            cropArea.outputDownOffset = CHECK_ODD(mapDownOffset);

            cropAreaArray.push_back(cropArea);
        }
    }

    // 计算缩放后的大小
    shared_ptr<CropResize> cropResize(new CropResize());
    uint32_t outBufferSize = cropResize->GetYuvOutputBufferSize(decodeOutputImage, resizeFactorW, resizeFactorH);
    // 用HIAI_DVPP_DMalloc申请内存
    uint8_t *outBuffer = (uint8_t *)HIAI_DVPP_DMalloc(outBufferSize);

    CropResizePara cropResizePara;
    for (int i = 0; i < cropAreaArray.size(); i++) {
        cropResizePara.cropAreaArray.push_back(cropAreaArray[i]);
    }
    // 此缩放参数，决定抠图输出图片(底图)的尺寸，请注意保证贴图区域在底图范围内，否则报错
    cropResizePara.resizeFactorW = resizeFactorW;
    cropResizePara.resizeFactorH = resizeFactorH;
    cropResizePara.inputFormat = INPUT_YUV420_SEMI_PLANNER_VU;
    cropResizePara.outputFormat = OUTPUT_YUV420SP_VU;
    //
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

    // crop and resize
    // 宽高的缩放系数范围为[1/32, 16]
    // 除8K缩放功能外，[10*6，4096*4096] 8K缩放仅支持yuv420, 最大分辨率4096*4096~8192*8192,
    // 输出分辨率支持16*16~4096*4096
    // 用户在设置宽高缩放系数的时候，除了考虑[1/32, 16]，还应考虑输出输出分辨率的限制
    float resizeFactorW = 1;
    float resizeFactorH = 1;

    shared_ptr<CropResizeOutputImage> cropResizeOutputImage(new CropResizeOutputImage);
    ret = CropImage(decodeOutputImage, cropResizeOutputImage, resizeFactorW, resizeFactorH);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[JPEGDResize] Resize image failed");
        return HIAI_ERROR;
    }

    ret = SendDataToDst(cropResizeOutputImage);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Send data to next engine falide.");
        return HIAI_ERROR;
    }
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[DvppCrop] end process!");
    return HIAI_OK;
}

DvppCrop::~DvppCrop()
{
}
