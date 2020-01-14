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
    // Dvpp���������ͼ���ǵ���VPC�ӿڣ����������źͿ�ͼ����Ҫ���ÿ�ͼ����ͼ����
    // ��8K���Ź����⣬��ͼ�������ͼ������С�ֱ���Ϊ10*6�����ֱ���Ϊ4096*4096
    // 8K���ţ����ֱ���֧��4096*4096~8192*8192����ʽ��֧��yuv420������ֱ���֧��16*16~4096*4096
    // CropArea ��װ��ͼ����ͼ��������Ľṹ��
    vector<CropArea> cropAreaArray;
    uint32_t blockWidth = (uint32_t)(decodeOutputImage->imgWidth * resizeFactorW) / CROP_NUM_COLROW;
    uint32_t blockHeigth = (uint32_t)(decodeOutputImage->imgHeight * resizeFactorH) / CROP_NUM_COLROW;
    uint32_t baseWidth = CHECK_ODD((uint32_t)(decodeOutputImage->imgWidth * resizeFactorW) - 1);
    uint32_t baseHeight = CHECK_ODD((uint32_t)(decodeOutputImage->imgHeight * resizeFactorH) - 1);
    // ����forѭ��������4����ͼ����������ͼ����cropArea push��cropAreaArray������һ���Խ���4��ͼ���������ͼ
    // �û����Ը��������������ÿ�ͼ����ͼ����һ���Կ�ͼ��ͼ��������Ϊ256
    for (int i = 0; i < CROP_NUM_COLROW; i++) {
        for (int j = 0; j < CROP_NUM_COLROW; j++) {
            CropArea cropArea;
            // ����Ϊ����������ԭͼ���ĵ�һ����Ϊ��ͼ����ʵ���Ͽ�ͼ����������仯����������ԭͼ��
            // ��ͼ���򣺶�ԭͼ����в������������������ԭͼ�ڡ�decodeOutputImageΪ�����������ΪVPCԭͼ��
            // ��ͼ����cropLeftOffset(��ƫ��) cropRightOffset(��ƫ��) cropUpOffset(��ƫ��) cropDownOffset(��ƫ��)
            cropArea.cropLeftOffset = CHECK_EVEN(blockWidth / CROP_NUM_COLROW);
            cropArea.cropRightOffset = CHECK_ODD((uint32_t)decodeOutputImage->imgWidth - blockWidth / CROP_NUM_COLROW - 1);
            cropArea.cropUpOffset = CHECK_EVEN(blockHeigth / CROP_NUM_COLROW);
            cropArea.cropDownOffset = CHECK_ODD((uint32_t)decodeOutputImage->imgHeight - blockHeigth / CROP_NUM_COLROW - 1);

            // ��ͼ���򣺶Կ�ͼ�������ڵ�ͼ�ĸ��ǵ�λ��, �û������������ã�λ�ò��ܳ��������ͼ�ķ�Χ
            // ������ͼ��������Ϊ���ź�ͼƬ
            // ��ͼ����outputLeftOffset(��ƫ��) outputRightOffset(��ƫ��) outputUpOffset(��ƫ��) outputDownOffset(��ƫ��)
            cropArea.outputLeftOffset = ALIGN_UP(i * blockWidth, WIDTH_ALIGNED);  // ����Ϊ16����
            // ��ֹ��ͼ��Χ������ͼ
            uint32_t tmpRightOffset = cropArea.outputLeftOffset + blockWidth - 1;
            uint32_t mapRightOffset = tmpRightOffset < baseWidth ? tmpRightOffset : baseWidth;
            cropArea.outputRightOffset = CHECK_ODD(mapRightOffset);

            cropArea.outputUpOffset = CHECK_EVEN(j * blockHeigth);
            // ��ֹ��ͼ��Χ������ͼ
            uint32_t tmpDownOffset = cropArea.outputUpOffset + blockHeigth - 1;
            uint32_t mapDownOffset = tmpDownOffset < baseHeight ? tmpDownOffset : baseHeight;
            cropArea.outputDownOffset = CHECK_ODD(mapDownOffset);

            cropAreaArray.push_back(cropArea);
        }
    }

    // �������ź�Ĵ�С
    shared_ptr<CropResize> cropResize(new CropResize());
    uint32_t outBufferSize = cropResize->GetYuvOutputBufferSize(decodeOutputImage, resizeFactorW, resizeFactorH);
    // ��HIAI_DVPP_DMalloc�����ڴ�
    uint8_t *outBuffer = (uint8_t *)HIAI_DVPP_DMalloc(outBufferSize);

    CropResizePara cropResizePara;
    for (int i = 0; i < cropAreaArray.size(); i++) {
        cropResizePara.cropAreaArray.push_back(cropAreaArray[i]);
    }
    // �����Ų�����������ͼ���ͼƬ(��ͼ)�ĳߴ磬��ע�Ᵽ֤��ͼ�����ڵ�ͼ��Χ�ڣ����򱨴�
    cropResizePara.resizeFactorW = resizeFactorW;
    cropResizePara.resizeFactorH = resizeFactorH;
    cropResizePara.inputFormat = INPUT_YUV420_SEMI_PLANNER_VU;
    cropResizePara.outputFormat = OUTPUT_YUV420SP_VU;
    //
    cropResizeOutputImage->outBufferSize = outBufferSize;
    cropResizeOutputImage->outBuffer = outBuffer;
    // ���÷�װ�õķ�����������
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
    // ���ߵ�����ϵ����ΧΪ[1/32, 16]
    // ��8K���Ź����⣬[10*6��4096*4096] 8K���Ž�֧��yuv420, ���ֱ���4096*4096~8192*8192,
    // ����ֱ���֧��16*16~4096*4096
    // �û������ÿ�������ϵ����ʱ�򣬳��˿���[1/32, 16]����Ӧ�����������ֱ��ʵ�����
    float resizeFactorW = 1;
    float resizeFactorH = 1;

    shared_ptr<CropResizeOutputImage> cropResizeOutputImage(new CropResizeOutputImage);
    ret = CropImage(decodeOutputImage, cropResizeOutputImage, resizeFactorW, resizeFactorH);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[JPEGDResize] Resize image failed");
        return HIAI_ERROR;
    }
    // free jpgd   
    cropResize->CbFreeJpeg();
    
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
