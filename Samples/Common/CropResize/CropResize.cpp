
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

#include "CropResize.h"

/* 0:do not change format, 1:change format(RGBA->RGB) */
const int TRANSFORM_NOT_CHANGE = 0;
const int TRANSFORM_CHANGE = 1;

/**
 * @brief DecodeJpeg
 * @param [in] : fileSize, the size of buff size
 * @param [in] : dataBuff, the jpeg data buffer
 * @param [out] : decodeOutputImage, the decode output
 * @return : HIAI_StatusT, HIAI_OK: success
 */
HIAI_StatusT CropResize::DecodeJpeg(const uint32_t fileSize, const std::shared_ptr<uint8_t> dataBuff,
                                    std::shared_ptr<DecodeOutputImage> decodeOutputImage)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "DecodeJpeg process start !");
    jpegdInData.jpegDataSize = fileSize;
    jpegdInData.jpegData = (unsigned char *)(dataBuff.get());

    // JpegdOut jpegdOutData;
    dvppapiCtlMsg.in = (void *)&jpegdInData;
    dvppapiCtlMsg.in_size = sizeof(jpegdInData);
    dvppapiCtlMsg.out = (void *)&jpegdOutData;
    dvppapiCtlMsg.out_size = sizeof(jpegdOutData);
    // create api
    int ret = CreateDvppApi(pidvppapi);
    if (ret != 0) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "create dvpp api fail.!\n");
        return ret;
    }
    // decode jpeg
    ret = DvppCtl(pidvppapi, DVPP_CTL_JPEGD_PROC, &dvppapiCtlMsg);
    if (ret != 0) {
        DestroyDvppApi(pidvppapi);
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "dvpp process error.\n");
        return HIAI_ERROR;
    } else {
        DestroyDvppApi(pidvppapi);
        HIAI_ENGINE_LOG(HIAI_IDE_INFO, "dvpp process success.\n");
    }

    decodeOutputImage->imgWidth = jpegdOutData.imgWidth;
    decodeOutputImage->imgHeight = jpegdOutData.imgHeight;
    decodeOutputImage->imgWidthAligned = jpegdOutData.imgWidthAligned;
    decodeOutputImage->imgHeightAligned = jpegdOutData.imgHeightAligned;
    decodeOutputImage->inBuffer = jpegdOutData.yuvData;
    decodeOutputImage->inBufferSize = jpegdOutData.yuvDataSize;

    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "DecodeJpeg process end !");
    return HIAI_OK;
}

/**
 * @brief Decode Png image
 * @param [in] : fileSize, the size of buff size
 * @param [in] : dataBuff, the png data buffer
 * @param [out] : decodeOutputImage, the decode output
 * @return : HIAI_StatusT, HIAI_OK: success
 */
HIAI_StatusT CropResize::DecodePng(const uint32_t fileSize, const std::shared_ptr<uint8_t> dataBuff, 
                                   std::shared_ptr<DecodeOutputImage> decodeOutputImage)
{
    inputPngData.inputData = (unsigned char *)(dataBuff.get());
    inputPngData.inputSize = fileSize;
    //0:do not change format, 1:change format
    inputPngData.transformFlag = TRANSFORM_CHANGE;

    dvppapiCtlMsg.in = (void*)&inputPngData;
    dvppapiCtlMsg.in_size = sizeof(inputPngData);
    dvppapiCtlMsg.out = (void*)&outputPngData;
    dvppapiCtlMsg.out_size = sizeof(outputPngData);

    int ret = CreateDvppApi(pidvppapi);
    if (ret != 0) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "create dvpp api fail.\n");
        return HIAI_ERROR;
    }

    ret = DvppCtl(pidvppapi, DVPP_CTL_PNGD_PROC, &dvppapiCtlMsg);
    if (ret != 0) {
        DestroyDvppApi(pidvppapi);
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "dvpp process error\n");
        return HIAI_ERROR;
    } else {
        DestroyDvppApi(pidvppapi);
        HIAI_ENGINE_LOG(HIAI_IDE_INFO, "dvpp process success\n");
    }

    decodeOutputImage->imgWidth = (uint32_t)outputPngData.width;
    decodeOutputImage->imgHeight = (uint32_t)outputPngData.high;
    decodeOutputImage->imgWidthAligned = outputPngData.widthAlign;
    decodeOutputImage->imgHeightAligned = outputPngData.highAlign;
    decodeOutputImage->inBuffer = (uint8_t*)outputPngData.address;
    decodeOutputImage->inBufferSize = outputPngData.outputSize;

    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "Decode png end.\n");
    return HIAI_OK;

}

/* construct image configure. */
void CropResize::ConstructImageConfigure(const std::shared_ptr<DecodeOutputImage> decodeOutputImage,
                                         std::shared_ptr<VpcUserImageConfigure> imageConfigure, const enum VpcInputFormat inputFormat,
                                         const enum VpcOutputFormat outputFormat)
{
    imageConfigure->bareDataAddr = decodeOutputImage->inBuffer;
    imageConfigure->bareDataBufferSize = decodeOutputImage->inBufferSize;
    imageConfigure->isCompressData = false;
    imageConfigure->widthStride = decodeOutputImage->imgWidthAligned;
    imageConfigure->heightStride = decodeOutputImage->imgHeightAligned;
    imageConfigure->inputFormat = inputFormat;
    imageConfigure->outputFormat = outputFormat;
    imageConfigure->yuvSumEnable = false;
    imageConfigure->cmdListBufferAddr = nullptr;
    imageConfigure->cmdListBufferSize = 0;
}

/* construct roi input configure. */
void CropResize::ConstructRoiInputConfigure(CropArea cropArea, VpcUserRoiInputConfigure *&inputConfigure)
{
    inputConfigure->cropArea.leftOffset = cropArea.cropLeftOffset;
    inputConfigure->cropArea.rightOffset = cropArea.cropRightOffset;
    inputConfigure->cropArea.upOffset = cropArea.cropUpOffset;
    inputConfigure->cropArea.downOffset = cropArea.cropDownOffset;
}

/* construct roi output configure. */
void CropResize::ConstructRoiOutputConfigure(const std::shared_ptr<DecodeOutputImage> decodeOutputImage,
                                             VpcUserRoiOutputConfigure *&outputConfigure,
                                             const CropResizePara cropResizePara, uint8_t *outBuffer, const uint32_t outBufferSize)
{
    uint32_t outWidth = (uint32_t)decodeOutputImage->imgWidth * cropResizePara.resizeFactorW;
    uint32_t outHeight = (uint32_t)decodeOutputImage->imgHeight * cropResizePara.resizeFactorH;
    uint32_t outWidthAligned = ALIGN_UP(outWidth, WIDTH_ALIGNED);
    uint32_t outHeightAligned = ALIGN_UP(outHeight, HEIGHT_ALIGNED);

    outputConfigure->widthStride = outWidthAligned;
    outputConfigure->heightStride = outHeightAligned;
    outputConfigure->addr = outBuffer;
    outputConfigure->bufferSize = outBufferSize;
    outputConfigure->outputArea.leftOffset = cropResizePara.cropAreaArray[0].outputLeftOffset;
    outputConfigure->outputArea.rightOffset = cropResizePara.cropAreaArray[0].outputRightOffset;
    outputConfigure->outputArea.upOffset = cropResizePara.cropAreaArray[0].outputUpOffset;
    outputConfigure->outputArea.downOffset = cropResizePara.cropAreaArray[0].outputDownOffset;
}

/* construct roi output configure. */
void CropResize::ConstructRoiOutputConfigure(const std::shared_ptr<DecodeOutputImage> decodeOutputImage,
                                             VpcUserRoiOutputConfigure *&outputConfigure,
                                             const CropResizePara cropResizePara,
                                             std::shared_ptr<CropResizeOutputImage> cropResizeOutputImage)
{
    uint32_t outWidth = (uint32_t)decodeOutputImage->imgWidth * cropResizePara.resizeFactorW;
    uint32_t outHeight = (uint32_t)decodeOutputImage->imgHeight * cropResizePara.resizeFactorH;
    uint32_t outWidthAligned = ALIGN_UP(outWidth, WIDTH_ALIGNED);
    uint32_t outHeightAligned = ALIGN_UP(outHeight, HEIGHT_ALIGNED);

    outputConfigure->widthStride = outWidthAligned;
    outputConfigure->heightStride = outHeightAligned;
    outputConfigure->addr = cropResizeOutputImage->outBuffer;
    outputConfigure->bufferSize = cropResizeOutputImage->outBufferSize;
    outputConfigure->outputArea.leftOffset = cropResizePara.cropAreaArray[0].outputLeftOffset;
    outputConfigure->outputArea.rightOffset = cropResizePara.cropAreaArray[0].outputRightOffset;
    outputConfigure->outputArea.upOffset = cropResizePara.cropAreaArray[0].outputUpOffset;
    outputConfigure->outputArea.downOffset = cropResizePara.cropAreaArray[0].outputDownOffset;

    cropResizeOutputImage->imgWidth = outWidth;
    cropResizeOutputImage->imgHeight = outHeight;
    cropResizeOutputImage->imgWidthAligned = outWidthAligned;
    cropResizeOutputImage->imgHeightAligned = outHeightAligned;
}

/**
 * @brief crop or resize image
 * @param [in] : decodeOutputImage, the decode output, the crop or resize input
 * @param [in] : cropResizePara, the param of crop or resize
 * @param [in] : outBufferSize, output buffer size
 * @param [out] : outBuffer, output buffer
 * @return : HIAI_StatusT, HIAI_OK: success
 */
HIAI_StatusT CropResize::CropResizeImage(const std::shared_ptr<DecodeOutputImage> decodeOutputImage,
                                         const CropResizePara cropResizePara,
                                         uint8_t *outBuffer, const uint32_t outBufferSize)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "CropResize process start !");
    ConstructImageConfigure(decodeOutputImage, imageConfigure, cropResizePara.inputFormat, cropResizePara.outputFormat);

    std::shared_ptr<VpcUserRoiConfigure> lastRoi;  // record the last roi configuration
    for (int i = 0; i < cropResizePara.cropAreaArray.size(); i++) {
        std::shared_ptr<VpcUserRoiConfigure> roiConfigure(new VpcUserRoiConfigure);
        roiConfigure->next = nullptr;
        VpcUserRoiInputConfigure *inputConfigure = &roiConfigure->inputConfigure;
        ConstructRoiInputConfigure(cropResizePara.cropAreaArray[i], inputConfigure);
        // set roi configuration
        VpcUserRoiOutputConfigure *outputConfigure = &roiConfigure->outputConfigure;
        CropResizePara outputCropResizePara;
        outputCropResizePara.cropAreaArray.push_back(cropResizePara.cropAreaArray[i]);
        outputCropResizePara.resizeFactorW = cropResizePara.resizeFactorW;
        outputCropResizePara.resizeFactorH = cropResizePara.resizeFactorH;
        outputCropResizePara.outputFormat = cropResizePara.outputFormat;
        outputCropResizePara.inputFormat = INPUT_YUV400; //not use, init 0

        ConstructRoiOutputConfigure(decodeOutputImage, outputConfigure, outputCropResizePara, outBuffer, outBufferSize);
        imageConfigure->roiConfigure = roiConfigure.get();
        // if it is the first one, set it to imageConfigure
        if (i == 0) {
            imageConfigure->roiConfigure = roiConfigure.get();
            lastRoi = roiConfigure;
        } else {
            lastRoi->next = roiConfigure.get();
            lastRoi = roiConfigure;
        }

        IDVPPAPI *pidvppapi = nullptr;
        int ret = CreateDvppApi(pidvppapi);
        if (ret != 0) {
            while (imageConfigure->roiConfigure != nullptr) {
                imageConfigure->roiConfigure = imageConfigure->roiConfigure->next;
            }
            return HIAI_ERROR;
        }
        // control msg
        dvppapi_ctl_msg dvppApiCtlMsg;
        dvppApiCtlMsg.in = static_cast<void *>(imageConfigure.get());
        dvppApiCtlMsg.in_size = sizeof(VpcUserImageConfigure);
        // resize the yuv
        ret = DvppCtl(pidvppapi, DVPP_CTL_VPC_PROC, &dvppApiCtlMsg);
        if (ret != 0) {
            HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "call vpc dvppctl process faild!\n");
            ret = DestroyDvppApi(pidvppapi);
            return HIAI_ERROR;
        } else {
            HIAI_ENGINE_LOG(HIAI_IDE_INFO, "call vpc dvppctl process success!\n");
        }
    }
    return HIAI_OK;
}

/**
 * @brief crop or resize image
 * @param [in] : decodeOutputImage, the decode output, the crop or resize input
 * @param [in] : cropResizePara, the param of crop or resize
 * @param [in] : outBufferSize, output buffer size
 * @param [out] : outBuffer, output buffer
 * @return : HIAI_StatusT, HIAI_OK: success
 */
HIAI_StatusT CropResize::CropResizeImage(const std::shared_ptr<DecodeOutputImage> decodeOutputImage,
                                         const CropResizePara cropResizePara,
                                         std::shared_ptr<CropResizeOutputImage> cropResizeOutputImage)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "CropResize process start !");
    ConstructImageConfigure(decodeOutputImage, imageConfigure, cropResizePara.inputFormat, cropResizePara.outputFormat);

    std::shared_ptr<VpcUserRoiConfigure> lastRoi;  // record the last roi configuration
    for (int i = 0; i < cropResizePara.cropAreaArray.size(); i++) {
        std::shared_ptr<VpcUserRoiConfigure> roiConfigure(new VpcUserRoiConfigure);
        roiConfigure->next = nullptr;
        VpcUserRoiInputConfigure *inputConfigure = &roiConfigure->inputConfigure;
        ConstructRoiInputConfigure(cropResizePara.cropAreaArray[i], inputConfigure);
        // set roi configuration
        VpcUserRoiOutputConfigure *outputConfigure = &roiConfigure->outputConfigure;
        CropResizePara outputCropResizePara;
        outputCropResizePara.cropAreaArray.push_back(cropResizePara.cropAreaArray[i]);
        outputCropResizePara.resizeFactorW = cropResizePara.resizeFactorW;
        outputCropResizePara.resizeFactorH = cropResizePara.resizeFactorH;
        outputCropResizePara.outputFormat = cropResizePara.outputFormat;
        outputCropResizePara.inputFormat = INPUT_YUV400; //not use, init 0

        ConstructRoiOutputConfigure(decodeOutputImage, outputConfigure, outputCropResizePara, cropResizeOutputImage);
        imageConfigure->roiConfigure = roiConfigure.get();
        // if it is the first one, set it to imageConfigure
        if (i == 0) {
            imageConfigure->roiConfigure = roiConfigure.get();
            lastRoi = roiConfigure;
        } else {
            lastRoi->next = roiConfigure.get();
            lastRoi = roiConfigure;
        }

        IDVPPAPI *pidvppapi = nullptr;
        int ret = CreateDvppApi(pidvppapi);
        if (ret != 0) {
            while (imageConfigure->roiConfigure != nullptr) {
                imageConfigure->roiConfigure = imageConfigure->roiConfigure->next;
            }
            return HIAI_ERROR;
        }
        // control msg
        dvppapi_ctl_msg dvppApiCtlMsg;
        dvppApiCtlMsg.in = static_cast<void *>(imageConfigure.get());
        dvppApiCtlMsg.in_size = sizeof(VpcUserImageConfigure);
        // resize the yuv
        ret = DvppCtl(pidvppapi, DVPP_CTL_VPC_PROC, &dvppApiCtlMsg);
        if (ret != 0) {
            HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "call vpc dvppctl process faild!\n");
            ret = DestroyDvppApi(pidvppapi);
            return HIAI_ERROR;
        } else {
            HIAI_ENGINE_LOG(HIAI_IDE_INFO, "call vpc dvppctl process success!\n");
        }
    }
    return HIAI_OK;
}


/**
 * @brief get mul crop or resize area according to the specify rol number or col number. It is only used for testing
 * @param [in] : outWidth, the width of image
 * @param [in] : outHeight, the height of image
 * @return : vector<CropArea>, the array of crop resize. the size is equal to rolNum*colNum
 */
vector<CropArea> CropResize::GetMulCropArea(uint32_t outWidth, uint32_t outHeight, uint32_t rolNum,
                                            uint32_t colNum)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "get MulCropArea process start !");

    uint32_t outWidthAligned = ALIGN_UP(outWidth, WIDTH_ALIGNED);
    uint32_t outHeightAligned = ALIGN_UP(outHeight, HEIGHT_ALIGNED);

    vector<CropArea> cropAreaArray;

    if ((rolNum <= 0) || (colNum <= 0)) {
        return cropAreaArray;
    }

    // calculate the crop size
    // Leave 16 pixels on the right for 16 alignment
    uint32_t blockWidth = outWidthAligned / rolNum;
    uint32_t blockHeigth = outHeightAligned / colNum;
    // crop area para
    uint32_t cropWidth = (uint32_t)blockWidth * CROP_BLOCK_FACTOR;
    uint32_t cropHeigth = (uint32_t)blockHeigth * CROP_BLOCK_FACTOR;
    // there is a gap between crop areas, the width of gap is equal to a half of the difference  value of
    // blockWidth and cropWidth, So as the height of it
    uint32_t widthStart = GAP_FACTOR * (blockWidth - cropWidth);
    uint32_t heigthStart = GAP_FACTOR * (blockHeigth - cropHeigth);

    for (int i = 0; i < rolNum; i++) {
        for (int j = 0; j < colNum; j++) {
            CropArea cropArea;
            cropArea.cropLeftOffset = ALIGN_UP(widthStart + i * blockWidth, WIDTH_ALIGNED);
            cropArea.cropRightOffset = CHECK_ODD(cropArea.cropLeftOffset + cropWidth - 1);
            cropArea.cropUpOffset = CHECK_EVEN(heigthStart + j * blockHeigth);
            cropArea.cropDownOffset = CHECK_ODD(cropArea.cropUpOffset + cropHeigth - 1);

            cropArea.outputLeftOffset = ALIGN_UP(widthStart + i * blockWidth, WIDTH_ALIGNED);
            cropArea.outputRightOffset = CHECK_ODD(cropArea.outputLeftOffset + cropWidth - 1);
            cropArea.outputUpOffset = CHECK_EVEN(heigthStart + j * blockHeigth);
            cropArea.outputDownOffset = CHECK_ODD(cropArea.outputUpOffset + cropHeigth - 1);
            cropAreaArray.push_back(cropArea);
        }
    }

    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "get multicropArea process end !");
    return cropAreaArray;
}

/**
 * @brief get one crop or reszie area according to image width and image height. It is only used for testing
 * @param [in] : outWidth, the width of image
 * @param [in] : outHeight, the height of image
 * @return : vector<CropArea>, the array of crop resize. the size is equal to 1
 */
vector<CropArea> CropResize::GetSingleArea(uint32_t outWidth, uint32_t outHeight)
{
    uint32_t singleRow = 1;
    uint32_t singleCol = 1;
    return this->GetMulCropArea(outWidth, outHeight, singleRow, singleCol);
}

/**
 * @brief get one resize area according to image width and image height. It is only used for testing
 * @param [in] : outWidth, the width of image
 * @param [in] : outHeight, the height of image
 * @return : vector<CropArea>, the array of crop resize. the size is equal to 1
 */
vector<CropArea> CropResize::GetResizeArea(uint32_t outWidth, uint32_t outHeight)
{
    return this->GetSingleArea(outWidth, outHeight);
}

/**
 * @brief get one crop area according to image width and image height. It is only used for testing
 * @param [in] : outWidth, the width of image
 * @param [in] : outHeight, the height of image
 * @return : vector<CropArea>, the array of crop resize. the size is equal to 1
 */
vector<CropArea> CropResize::GetCropArea(uint32_t outWidth, uint32_t outHeight)
{
    return this->GetSingleArea(outWidth, outHeight);
}

/**
 * @brief calculate the buffer size of yuv output, according to resize scale
 * @param [in] : decodeOutputImage, the width of image
 * @param [in] : resizeFactorW, the resize scale of width
 * @param [in] : resizeFactorH, the resize scale of height
 * @return : uint32_t, the yuv buffer size
 */
uint32_t CropResize::GetYuvOutputBufferSize(const std::shared_ptr<DecodeOutputImage> decodeOutputImage,
                                            const float resizeFactorW, const float resizeFactorH)
{
    uint32_t outWidth = (uint32_t)decodeOutputImage->imgWidth * resizeFactorW;
    uint32_t outHeight = (uint32_t)decodeOutputImage->imgHeight * resizeFactorH;
    uint32_t outWidthAligned = ALIGN_UP(outWidth, WIDTH_ALIGNED);
    uint32_t outHeightAligned = ALIGN_UP(outHeight, HEIGHT_ALIGNED);

    return outWidthAligned * outHeightAligned * YUV_FACTOR / YUV_DIVISOR;
}

void CropResize::CbFreeJpeg()
{
    jpegData.cbFree();
    jpegData.yuvData = nullptr;
}

CropResize::~CropResize()
{
}
