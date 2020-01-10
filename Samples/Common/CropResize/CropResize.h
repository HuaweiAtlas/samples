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

#ifndef COMMON_VPCCONTROL_H
#define COMMON_VPCCONTROL_H

#include "Common.h"
#include <hiaiengine/api.h>
#include <string.h>
#include <vector>
#include <hiaiengine/engine.h>
#include "dvpp/Vpc.h"
#include "hiaiengine/multitype_queue.h"
#include "hiaiengine/ai_memory.h"
#include "dvpp/idvppapi.h"
#include "hiaiengine/c_graph.h"

using namespace std;

#define CHECK_ODD(NUM)  ((((NUM) % (2)) != (0)) ? (NUM) : ((NUM) - (1)))
#define CHECK_EVEN(NUM) ((((NUM) % (2)) == (0)) ? (NUM) : ((NUM) - (1)))

/* Aligned length in C30. it would be 128*16 in C10. */
static const int WIDTH_ALIGNED = 16;
static const int HEIGHT_ALIGNED = 2;

/* they are used in calculating multiple crop area. */
static const float CROP_BLOCK_FACTOR = 1;
static const float GAP_FACTOR = 0.5;

/* they are used in calculating YUV buffer. */
static const int YUV_FACTOR = 3;
static const int YUV_DIVISOR = 2;

/* struct of crop area. */
typedef struct CropArea {
    /* crop area of user input */
    uint32_t cropLeftOffset;
    uint32_t cropRightOffset;
    uint32_t cropUpOffset;
    uint32_t cropDownOffset;
    /* crop output area. */
    uint32_t outputLeftOffset;
    uint32_t outputRightOffset;
    uint32_t outputUpOffset;
    uint32_t outputDownOffset;
} CropAreaT;

/* struct of output image. */
typedef struct DecodeOutputImage {
    uint32_t imgWidth; /* the width of output image. */
    uint32_t imgHeight; /* the height og output image. */
    uint32_t imgWidthAligned; /* the aligned with. */
    uint32_t imgHeightAligned; /* the aligned height. */
    unsigned char *inBuffer; /* output image buffer. */
    uint32_t inBufferSize; /* output image buffer size. */
} DecodeOutputImageT;

/* struct of CropResizeOutputImage image. */
typedef struct CropResizeOutputImage {
    uint32_t imgWidth; /* the width of output image. */
    uint32_t imgHeight; /* the height og output image. */
    uint32_t imgWidthAligned; /* the aligned with. */
    uint32_t imgHeightAligned; /* the aligned height. */
    unsigned char *outBuffer; /* output image buffer. */
    uint32_t outBufferSize; /* output image buffer size. */
} CropResizeOutputImage;

/* the param of crop and resize image. */
typedef struct CropResizePara {
    vector<CropArea> cropAreaArray; /* the array of crop area. */
    float resizeFactorW; /* the resize scale of width. */
    float resizeFactorH; /* the resize scale of height. */
    enum VpcInputFormat inputFormat; /* the decoder input image format. */
    enum VpcOutputFormat outputFormat; /* the decoder output image format. */
} CropResizeParaT;

class CropResize {
public:
    CropResize():imageConfigure(new VpcUserImageConfigure), roiConfigure(new VpcUserRoiConfigure)
    {
    }

    /**
     * @brief Decode  Jpeg Image
     * @param [in] : fileSize, the size of buff size
     * @param [in] : dataBuff, the jpeg data buffer
     * @param [out] : decodeOutputImage, the decode output
     * @return : HIAI_StatusT, HIAI_OK: success
     */
    HIAI_StatusT DecodeJpeg(const uint32_t fileSize, const std::shared_ptr<uint8_t> dataBuff,
                            std::shared_ptr<DecodeOutputImage> decodeOutputImage);

    /**
     * @brief Decode Png image
     * @param [in] : fileSize, the size of buff size
     * @param [in] : dataBuff, the png data buffer
     * @param [out] : decodeOutputImage, the decode output
     * @return : HIAI_StatusT, HIAI_OK: success
     */
    HIAI_StatusT DecodePng(const uint32_t fileSize, const std::shared_ptr<uint8_t> dataBuff,
                            std::shared_ptr<DecodeOutputImage> decodeOutputImage);

    /**
     * @brief crop or resize image
     * @param [in] : decodeOutputImage, the decode output, the crop or resize input
     * @param [in] : cropResizePara, the param of crop or resize
     * @param [in] : outBufferSize, output buffer size
     * @param [out] : outBuffer, output buffer
     * @return : HIAI_StatusT, HIAI_OK: success
     */
    HIAI_StatusT CropResizeImage(const std::shared_ptr<DecodeOutputImage> decodeOutputImage,
                                 const CropResizePara cropResizePara,
                                 uint8_t *outBuffer, const uint32_t outBufferSize);

    /**
     * @brief crop or resize image
     * @param [in] : decodeOutputImage, the decode output, the crop or resize input
     * @param [in] : cropResizePara, the param of crop or resize
     * @param [in] : CropResizeOutputImage, CropResizeOuput
     * @return : HIAI_StatusT, HIAI_OK: success
    */
    HIAI_StatusT CropResizeImage(const std::shared_ptr<DecodeOutputImage> decodeOutputImage,
                                 const CropResizePara cropResizePara,
                                 std::shared_ptr<CropResizeOutputImage> cropResizeOutputImage);

    /**
     * @brief get mul crop or resize area according to the specify rol number or col number. It is only used for testing
     * @param [in] : outWidth, the width of image
     * @param [in] : outHeight, the height of image
     * @param [in] : rolNum
     * @param [in] : colNum
     * @return : vector<CropArea>, the array of crop resize. the size is equal to rolNum*colNum
     */
    vector<CropArea> GetMulCropArea(uint32_t outWidth, uint32_t outHeight, uint32_t rolNum, uint32_t colNum);

    /**
     * @brief get one crop or reszie area according to image width and image height. It is only used for testing
     * @param [in] : outWidth, the width of image
     * @param [in] : outHeight, the height of image
     * @return : vector<CropArea>, the array of crop resize. the size is equal to 1
     */
    vector<CropArea> GetSingleArea(uint32_t outWidth, uint32_t outHeight);

    /**
     * @brief get one resize area according to image width and image height. It is only used for testing
     * @param [in] : outWidth, the width of image
     * @param [in] : outHeight, the height of image
     * @return : vector<CropArea>, the array of crop resize. the size is equal to 1
     */
    vector<CropArea> GetResizeArea(uint32_t outWidth, uint32_t outHeight);

    /**
     * @brief get one crop area according to image width and image height. It is only used for testing
     * @param [in] : outWidth, the width of image
     * @param [in] : outHeight, the height of image
     * @return : vector<CropArea>, the array of crop resize. the size is equal to 1
     */
    vector<CropArea> GetCropArea(uint32_t outWidth, uint32_t outHeight);

    /**
     * @brief calculate the buffer size of yuv output, according to resize scale
     * @param [in] : decodeOutputImage, the width of image
     * @param [in] : resizeFactorW, the resize scale of width
     * @param [in] : resizeFactorH, the resize scale of height
     * @return : uint32_t, the yuv buffer size
     */
    uint32_t GetYuvOutputBufferSize(const std::shared_ptr<DecodeOutputImage> decodeOutputImage,
                                    const float resizeFactorW, const float resizeFactorH);
    // Free Jpgd
    void CbFreeJpeg();
	
    // Free PngD
    void CbFreePng();
	
    ~CropResize();

private:
    // set ImageConfigure
    void ConstructImageConfigure(const std::shared_ptr<DecodeOutputImage> decodeOutputImage,
                                 std::shared_ptr<VpcUserImageConfigure> imageConfigure, const enum VpcInputFormat inputFormat,
                                 const enum VpcOutputFormat outputFormat);
    // set roi configure
    void ConstructRoiInputConfigure(CropArea cropArea, VpcUserRoiInputConfigure *&inputConfigure);
    // set output configure
    void ConstructRoiOutputConfigure(const std::shared_ptr<DecodeOutputImage> decodeOutputImage,
                                     VpcUserRoiOutputConfigure *&outputConfigure,
                                     const CropResizePara cropResizePara, uint8_t *outBuffer, const uint32_t outBufferSize);

    void ConstructRoiOutputConfigure(const std::shared_ptr<DecodeOutputImage> decodeOutputImage,
                                     VpcUserRoiOutputConfigure *&outputConfigure,
                                     const CropResizePara cropResizePara,
                                     std::shared_ptr<CropResizeOutputImage> cropResizeOutputImage);

    // Private implementation a member variable, which is used to cache the input queue
    // hiai::MultiTypeQueue input_que_;
    std::shared_ptr<VpcUserImageConfigure> imageConfigure;  // the input picture configuration
    struct JpegdIn jpegdInData;                             // crop input data
    struct JpegdOut jpegdOutData;                           // jpeg crop output data
    struct PngInputInfoAPI inputPngData;                    // jpeg crop input data
    struct PngOutputInfoAPI outputPngData;                  // png crop output data
    dvppapi_ctl_msg dvppapiCtlMsg;                          // png control msg
    IDVPPAPI *pidvppapi = NULL;
    std::shared_ptr<VpcUserRoiConfigure> roiConfigure;  // the crop area configuration;
};

#endif  // COMMON_VPCCONTROL_H
