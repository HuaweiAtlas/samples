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

#include "DecodeEngine.h"
#include "error_code.h"
#include <hiaiengine/data_type.h>
#include "hiaiengine/log.h"
#include "hiaiengine/data_type_reg.h"
#include "hiaiengine/api.h"
#include "hiaiengine/ai_model_manager.h"
#include <string.h>
#include <vector>
#include <stdio.h>
#include <map>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <queue>
#include <malloc.h>
#include <math.h>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <time.h>
#include <unistd.h>

const int FRAME_SKIP_INTERVAL = 1;

HIAI_REGISTER_SERIALIZE_FUNC("StreamRawData", StreamRawData, StreamRawDataSerialize, StreamRawDataDeserialize);

using namespace std;

void DecodeEngine::VdecResultCallback(FRAME *frame, void *hiai_data)
{
    /* judge parameter */
    if (hiai_data == NULL) {
        HIAI_ENGINE_LOG(APP_ERROR, " input parameter of VdecResultCallback error!");
        return;
    }

    DecodeEngine *enginePtr = static_cast<DecodeEngine *>(hiai_data);
    enginePtr->frameId++;

    /* Get decode video information */
    StreamInfo decodeH26xInfo = enginePtr->frameInfo;
    decodeH26xInfo.frameId = enginePtr->frameId;

    /* pick one frame every FRAME_SKIP_INTERVAL frame */
    if ((decodeH26xInfo.frameId % FRAME_SKIP_INTERVAL) != 0) {
        return;
    }

    /*
     * convert hbfc to yuv420
 */
    std::vector<ImageInfo> cropImageList;
    if (HIAI_OK != enginePtr->VdecImageResize(frame, cropImageList)) {
        HIAI_ENGINE_LOG(APP_ERROR, " VdecImageResize of VdecResultCallback error!");
        return;
    }

    /* construct data transmission struct */
    std::shared_ptr<DeviceStreamData> deviceStreamData = std::make_shared<DeviceStreamData>();
    deviceStreamData->info = decodeH26xInfo;
    deviceStreamData->imgOrigin = cropImageList[0];
    deviceStreamData->detectImg = cropImageList[1];

    if (HIAI_OK != enginePtr->SendData(0, "DeviceStreamData",
                                       std::static_pointer_cast<void>(deviceStreamData))) {
        HIAI_ENGINE_LOG(APP_ERROR, " [DecodeEngine] SendData error!");
        return;
    }

    return;
}

void DecodeEngine::VdecErrorCallback(VDECERR *vdecErr)
{
    HIAI_ENGINE_LOG(APP_ERROR, "VdecErrorCallback vdec error!");
}

HIAI_StatusT DecodeEngine::Init(const AIConfig &config, const std::vector<AIModelDescription> &model_desc)
{
    HIAI_StatusT ret = HIAI_OK;
    /* create the vdec object */
    if (piDvppApiVdec == NULL) {
        ret = CreateVdecApi(piDvppApiVdec, 0);
        if ((ret != HIAI_OK) && (piDvppApiVdec == NULL)) {
            HIAI_ENGINE_LOG(APP_ERROR, "fail to intialize dvpp api!");
            return HIAI_ERROR;
        }
    }

    /* create the JpegD object */
    if (piDvppApiJpegD == NULL) {
        ret = CreateDvppApi(piDvppApiJpegD);
        if ((ret != HIAI_OK) && (piDvppApiJpegD == NULL)) {
            HIAI_ENGINE_LOG(APP_ERROR, "fail to intialize JpegD api!");
            return HIAI_ERROR;
        }
    }

    /* create the vpc object */
    if (piDvppApiVpc == NULL) {
        ret = CreateDvppApi(piDvppApiVpc);
        if ((ret != HIAI_OK) && (piDvppApiVpc == NULL)) {
            HIAI_ENGINE_LOG(APP_ERROR, "fail to intialize vpc api!");
            return HIAI_ERROR;
        }
    }

    return HIAI_OK;
}

DecodeEngine::~DecodeEngine()
{
    /* Destroy vdec object */
    if (piDvppApiVdec != NULL) {
        HIAI_ENGINE_LOG(APP_ERROR, "Destroy vdec api!");
        DestroyVdecApi(piDvppApiVdec, 0);
        piDvppApiVdec = NULL;
    }

    /* Destroy JpegD object */
    if (piDvppApiJpegD != NULL) {
        HIAI_ENGINE_LOG(APP_ERROR, "Destroy JpegD api!");
        DestroyDvppApi(piDvppApiJpegD);
        piDvppApiJpegD = NULL;
    }

    /* Destroy vpc object */
    if (piDvppApiVpc != NULL) {
        HIAI_ENGINE_LOG(APP_ERROR, "Destroy vpc api!");
        DestroyDvppApi(piDvppApiVpc);
        piDvppApiVpc = NULL;
    }
}

/**
* @ingroup hiaiengine
*
**/
HIAI_IMPL_ENGINE_PROCESS("DecodeEngine", DecodeEngine, DECODE_INPUT_SIZE)
{
    HIAI_ENGINE_LOG(APP_INFO, "[DecodeEngine] start");

    std::shared_ptr<StreamRawData> streamRawData = std::static_pointer_cast<StreamRawData>(arg0);
    if (streamRawData == nullptr) {
        HIAI_ENGINE_LOG(APP_ERROR, "[DecodeEngine] Invalid input parameter of engine!");
        return HIAI_ERROR;
    }

    /* jpeg/jpg decode */
    if (0 == streamRawData->info.format) {
        /* construct data transmission struct */
        std::shared_ptr<DeviceStreamData> deviceStreamData = std::make_shared<DeviceStreamData>();
        deviceStreamData->info = streamRawData->info;

        /* Judge whether jpg file is over */
        if (deviceStreamData->info.isEOS != 1) {
            /* decode and resize */
            if (HIAI_OK != JpgImageDecode(streamRawData->buf.data.get(), streamRawData->buf.len_of_byte,
                                          deviceStreamData->imgOrigin, deviceStreamData->detectImg)) {
                HIAI_ENGINE_LOG(APP_ERROR, "[DecodeEngine] JpgImageDecode() error!");
                return HIAI_ERROR;
            }
        }

        if (HIAI_OK != hiai::Engine::SendData(0, "DeviceStreamData",
                                              std::static_pointer_cast<void>(deviceStreamData))) {
            HIAI_ENGINE_LOG(APP_ERROR, "[DecodeEngine] SendData error!");
            return HIAI_ERROR;
        }

        return HIAI_OK;
    }
    /* H26* decode */
    else {
        if (HIAI_OK != DecodeH26xVideo(streamRawData)) {
            HIAI_ENGINE_LOG(APP_ERROR, "[DecodeEngine] DecodeH26xVideo() error!");
            return HIAI_ERROR;
        }
    }

    HIAI_ENGINE_LOG(APP_INFO, "[DecodeEngine] end");
    return HIAI_OK;
}
const int VIDEO_FRMAT_LEN = 10;
HIAI_StatusT DecodeEngine::DecodeH26xVideo(std::shared_ptr<StreamRawData> streamRawData)
{
    HIAI_ENGINE_LOG(APP_INFO, "VDEC begin decode %d frame", streamRawData->info.frameId);
    /* define parameters for DVPP Vdec */
    vdec_in_msg vdecMsg;

    /* decode the H26x video */
    // set input fromat: h264, h265 warning, please define enum
    if (streamRawData->info.format == FORMAT_H264) {
        strncpy_s(vdecMsg.video_format, VIDEO_FRMAT_LEN, "h264", sizeof("h264"));
    } else {
        strncpy_s(vdecMsg.video_format, VIDEO_FRMAT_LEN, "h265", sizeof("h264"));
    }

    // set output format: yuv420sp_UV(default)=nv12, yuv420sp_VU=nv21
    strncpy_s(vdecMsg.image_format, VIDEO_FRMAT_LEN, "nv12", sizeof("h264"));

    vdecMsg.in_buffer = (char *)streamRawData->buf.data.get();
    vdecMsg.in_buffer_size = streamRawData->buf.len_of_byte;

    vdecMsg.channelId = streamRawData->info.channelId;
    vdecMsg.isEOS = false;  // (streamRawData->info.isEOS == 1) ? true : false;
    this->frameInfo = streamRawData->info;
    this->frameInfo.isEOS = 0;
    vdecMsg.hiai_data = (void *)this;  // if use hiai_data_sp, set hiai_data as NULL

    vdecMsg.call_back = DecodeEngine::VdecResultCallback;
    vdecMsg.err_report = DecodeEngine::VdecErrorCallback;

    dvppapi_ctl_msg dvppapiCtlMsg;
    dvppapiCtlMsg.in_size = sizeof(vdec_in_msg);
    dvppapiCtlMsg.in = (void *)&vdecMsg;

    if (0 != VdecCtl(piDvppApiVdec, DVPP_CTL_VDEC_PROC, &dvppapiCtlMsg, 0)) {
        HIAI_ENGINE_LOG(APP_ERROR, " VdecCtl of DecodeH26xVideo() error!");
        return HIAI_ERROR;
    }

    /* if the video stream end, set isEOS, and call VdecCtl() to output the frame in the cacahe of Vdec */
    if (streamRawData->info.isEOS == 1) {
        vdecMsg.isEOS = true;
        if (0 != VdecCtl(piDvppApiVdec, DVPP_CTL_VDEC_PROC, &dvppapiCtlMsg, 0)) {
            HIAI_ENGINE_LOG(APP_ERROR, " VdecCtl of DecodeH26xVideo() error!");
            return HIAI_ERROR;
        }

        HIAI_ENGINE_LOG(APP_ERROR, "[DecodeEngine] Clear buffer!");
        /* construct data transmission struct */
        std::shared_ptr<DeviceStreamData> deviceStreamData = std::make_shared<DeviceStreamData>();
        deviceStreamData->info = streamRawData->info;
        if (HIAI_OK != hiai::Engine::SendData(0, "DeviceStreamData",
                                              std::static_pointer_cast<void>(deviceStreamData))) {
            HIAI_ENGINE_LOG(APP_ERROR, "[DecodeEngine] SendData error!");
            return HIAI_ERROR;
        }
    }

    HIAI_ENGINE_LOG(APP_INFO, "VDEC decode end");

    return HIAI_OK;
}

HIAI_StatusT DecodeEngine::JpgImageDecode(uint8_t *jpgImageDataBuf, int jpgImageDataLength,
                                          ImageInfo &cropRawImage, ImageInfo &cropDetectImage)
{
    // prepare msg
    JpegdIn jpegdInData;    // input data
    JpegdOut jpegdOutData;  // output data

    jpegdInData.isVBeforeU = true;    // currently, only support V before U, reserved
    jpegdInData.isYUV420Need = true;  // output format: yuv420sp

    /*
    * input buffer need that 128 aligned and length + 8
    * In host, use HiAi_DMalloc to apply for memory, and add 8 to the length, otherwise refer to the interface documentation of DVPP
    */
    jpegdInData.jpegDataSize = jpgImageDataLength;  // input length of jpg raw data, need to add 8;
    jpegdInData.jpegData = jpgImageDataBuf;         // 128 aligned

    // dvppCtl �1�7�1�7�1�7�1�7�1�7�1�7�1�7�1�7
    dvppapi_ctl_msg dvppApiCtlMsg;
    dvppApiCtlMsg.in = (void *)&jpegdInData;
    dvppApiCtlMsg.in_size = sizeof(jpegdInData);
    dvppApiCtlMsg.out = (void *)&jpegdOutData;
    dvppApiCtlMsg.out_size = sizeof(jpegdOutData);

    // call DvppCtl to process
    if (0 != DvppCtl(piDvppApiJpegD, DVPP_CTL_JPEGD_PROC, &dvppApiCtlMsg)) {
        HIAI_ENGINE_LOG(APP_ERROR, "JpgImageDecode jpg decode error�1�7�1�7");
        DestroyDvppApi(piDvppApiJpegD);  // destory dvppapi
        piDvppApiJpegD = NULL;
        return HIAI_ERROR;
    }

    std::vector<ImageInfo> cropImageList;
    if (HIAI_OK != JpgImageResize(jpegdOutData, cropImageList)) {
        HIAI_ENGINE_LOG(APP_ERROR, "JpgImageResize() error!");
        return HIAI_ERROR;
    }

    // output data
    cropRawImage = cropImageList[0];
    cropDetectImage = cropImageList[1];

    // free buffer
    jpegdOutData.cbFree();
    return HIAI_OK;
}

HIAI_StatusT DecodeEngine::JpgImageResize(JpegdOut &jpegdOutData, std::vector<ImageInfo> &cropImageList)
{
    /*
    * vpc, change format and resize
 */
    /* Construct VPC input parameter */
    std::shared_ptr<VpcUserImageConfigure> imageConfigure(new VpcUserImageConfigure);
    imageConfigure->bareDataAddr = jpegdOutData.yuvData;
    imageConfigure->bareDataBufferSize = jpegdOutData.yuvDataSize;
    imageConfigure->isCompressData = false;
    imageConfigure->widthStride = jpegdOutData.imgWidthAligned;
    imageConfigure->heightStride = jpegdOutData.imgHeightAligned;
    imageConfigure->inputFormat = INPUT_YUV420_SEMI_PLANNER_VU;
    imageConfigure->outputFormat = OUTPUT_YUV420SP_UV;
    imageConfigure->yuvSumEnable = false;
    imageConfigure->cmdListBufferAddr = nullptr;
    imageConfigure->cmdListBufferSize = 0;
    std::shared_ptr<VpcUserRoiConfigure> lastRoi;

    /* ouput 0: raw image, convert format from INPUT_YUV420_SEMI_PLANNER_VU to OUTPUT_YUV420SP_UV */
    /* ouput 1: 
     * resized image for detect network input, convert format from INPUT_YUV420_SEMI_PLANNER_VU to OUTPUT_YUV420SP_UV */
    std::shared_ptr<VpcUserRoiConfigure> roiConfigureOutput0(new VpcUserRoiConfigure);
    std::shared_ptr<VpcUserRoiConfigure> roiConfigureOutput1(new VpcUserRoiConfigure);
    imageConfigure->roiConfigure = roiConfigureOutput0.get();
    roiConfigureOutput0->next = roiConfigureOutput1.get();
    roiConfigureOutput1->next = nullptr;

    /* ouput 0: raw image */
    VpcUserRoiInputConfigure *inputConfigure = &roiConfigureOutput0->inputConfigure;
    // set map area: the entire picture
    inputConfigure->cropArea.leftOffset = 0;                                   // even
    inputConfigure->cropArea.rightOffset = CHECK_ODD(jpegdOutData.imgWidth);   // odd
    inputConfigure->cropArea.upOffset = 0;                                     // even
    inputConfigure->cropArea.downOffset = CHECK_ODD(jpegdOutData.imgHeight);   // odd

    // Construct output buffer
    uint8_t *output0Buffer = nullptr;
    HIAI_StatusT ret = hiai::HIAIMemory::HIAI_DVPP_DMalloc(jpegdOutData.yuvDataSize, (void *&)output0Buffer);
    if (ret != HIAI_OK || output0Buffer == nullptr) {
        HIAI_ENGINE_LOG(APP_ERROR, " HIAI_DVPP_DMalloc of JpgImageResize() error!");
        return HIAI_ERROR;
    }
    VpcUserRoiOutputConfigure *outputConfigure = &roiConfigureOutput0->outputConfigure;
    outputConfigure->addr = output0Buffer;  // align to 128
    outputConfigure->bufferSize = jpegdOutData.yuvDataSize;
    outputConfigure->widthStride = jpegdOutData.imgWidthAligned;    // align to 128
    outputConfigure->heightStride = jpegdOutData.imgHeightAligned;  // align to 16

    // set crop area:
    outputConfigure->outputArea.leftOffset = 0;  // align to 16
    outputConfigure->outputArea.rightOffset = CHECK_ODD(jpegdOutData.imgWidth);   // odd
    outputConfigure->outputArea.upOffset = 0;
    outputConfigure->outputArea.downOffset = CHECK_ODD(jpegdOutData.imgHeight);   // odd

    /* ouput 1: resized image for detect network input */
    inputConfigure = &roiConfigureOutput1->inputConfigure;
    // set map area: the entire picture
    inputConfigure->cropArea = roiConfigureOutput0->inputConfigure.cropArea;  // same as output 0

    // Construct output buffer
    outputConfigure = &roiConfigureOutput1->outputConfigure;
    outputConfigure->widthStride = ALIGN_UP(g_detectInputWidth, 128);  // align to 128
    outputConfigure->heightStride = ALIGN_UP(g_detectInputHeight,  16);  // align to 16
    outputConfigure->bufferSize = outputConfigure->widthStride * outputConfigure->heightStride * YUV_BYTES; // yuv420sp_UV
    uint8_t *output1Buffer = nullptr;
    ret = hiai::HIAIMemory::HIAI_DVPP_DMalloc(outputConfigure->bufferSize, (void *&)output1Buffer);
    if (ret != HIAI_OK || output1Buffer == nullptr) {
        HIAI_ENGINE_LOG(APP_ERROR, " HIAI_DVPP_DMalloc of JpgImageResize() error!");
        hiai::HIAIMemory::HIAI_DVPP_DFree(roiConfigureOutput0->outputConfigure.addr);
        return HIAI_ERROR;
    }
    outputConfigure->addr = output1Buffer;  // align to 128

    // set crop area:
    outputConfigure->outputArea.leftOffset = 0;  // align to 16
    outputConfigure->outputArea.rightOffset = CHECK_ODD(g_detectInputWidth);
    outputConfigure->outputArea.upOffset = 0;
    outputConfigure->outputArea.downOffset = CHECK_ODD(g_detectInputHeight);

    /* process of VPC */
    dvppapi_ctl_msg dvppApiCtlMsg;
    dvppApiCtlMsg.in = static_cast<void *>(imageConfigure.get());
    dvppApiCtlMsg.in_size = sizeof(VpcUserImageConfigure);

    if (0 != DvppCtl(piDvppApiVpc, DVPP_CTL_VPC_PROC, &dvppApiCtlMsg)) {
        HIAI_ENGINE_LOG(APP_ERROR, " vpc of JpgImageResize() error!");
        hiai::HIAIMemory::HIAI_DVPP_DFree(roiConfigureOutput0->outputConfigure.addr);
        hiai::HIAIMemory::HIAI_DVPP_DFree(roiConfigureOutput1->outputConfigure.addr);
        return HIAI_ERROR;
    }

    /* Get output image */
    while (imageConfigure->roiConfigure != nullptr) {
        ImageInfo img;
        img.format = OUTPUT_YUV420SP_UV;
        img.width = imageConfigure->roiConfigure->outputConfigure.outputArea.rightOffset;
        img.height = imageConfigure->roiConfigure->outputConfigure.outputArea.downOffset;
        img.widthAligned = imageConfigure->roiConfigure->outputConfigure.widthStride;
        img.heightAligned = imageConfigure->roiConfigure->outputConfigure.heightStride;
        img.buf.data = std::shared_ptr<uint8_t>(imageConfigure->roiConfigure->outputConfigure.addr,
                                                [](std::uint8_t *data) { hiai::HIAIMemory::HIAI_DVPP_DFree(data); });
        img.buf.len_of_byte = imageConfigure->roiConfigure->outputConfigure.bufferSize;

        /* update */
        cropImageList.push_back(img);
        imageConfigure->roiConfigure = imageConfigure->roiConfigure->next;
    }

    return HIAI_OK;
}

HIAI_StatusT DecodeEngine::VdecImageResize(FRAME *frame, std::vector<ImageInfo> &cropImageList)
{
    /*
    * vpc, change format and resize
 */
    /* Construct VPC input parameter */
    std::shared_ptr<VpcUserImageConfigure> imageConfigure(new VpcUserImageConfigure);
    imageConfigure->bareDataAddr = nullptr;
    imageConfigure->bareDataBufferSize = 0;
    // set input frame format
    imageConfigure->isCompressData = true;
    imageConfigure->widthStride = frame->width;
    imageConfigure->heightStride = frame->height;
    string imageFormat(frame->image_format);
    if (imageFormat == "nv12") {
        imageConfigure->inputFormat = INPUT_YUV420_SEMI_PLANNER_UV;
    } else {
        imageConfigure->inputFormat = INPUT_YUV420_SEMI_PLANNER_VU;
    }
  
    // set hfbc input address
    VpcCompressDataConfigure *compressDataConfigure = &imageConfigure->compressDataConfigure;
    uintptr_t baseAddr = (uintptr_t)frame->buffer;
    compressDataConfigure->lumaHeadAddr = baseAddr + frame->offset_head_y;
    compressDataConfigure->chromaHeadAddr = baseAddr + frame->offset_head_c;
    compressDataConfigure->lumaPayloadAddr = baseAddr + frame->offset_payload_y;
    compressDataConfigure->chromaPayloadAddr = baseAddr + frame->offset_payload_c;
    compressDataConfigure->lumaHeadStride = frame->stride_head;
    compressDataConfigure->chromaHeadStride = frame->stride_head;
    compressDataConfigure->lumaPayloadStride = frame->stride_payload;
    compressDataConfigure->chromaPayloadStride = frame->stride_payload;

    imageConfigure->outputFormat = OUTPUT_YUV420SP_UV;
    imageConfigure->yuvSumEnable = false;
    imageConfigure->cmdListBufferAddr = nullptr;
    imageConfigure->cmdListBufferSize = 0;
    std::shared_ptr<VpcUserRoiConfigure> lastRoi;

    /* ouput 0: raw image, convert format from INPUT_YUV420_SEMI_PLANNER_VU to OUTPUT_YUV420SP_UV */
    /* ouput 1: resized image for detect network input, convert format from INPUT_YUV420_SEMI_PLANNER_VU to OUTPUT_YUV420SP_UV */
    std::shared_ptr<VpcUserRoiConfigure> roiConfigureOutput0(new VpcUserRoiConfigure);
    std::shared_ptr<VpcUserRoiConfigure> roiConfigureOutput1(new VpcUserRoiConfigure);
    imageConfigure->roiConfigure = roiConfigureOutput0.get();
    roiConfigureOutput0->next = roiConfigureOutput1.get();
    roiConfigureOutput1->next = nullptr;

    /* ouput 0: raw image */
    VpcUserRoiInputConfigure *inputConfigure = &roiConfigureOutput0->inputConfigure;
    // set map area: the entire picture
    inputConfigure->cropArea.leftOffset = 0;                            // even
    inputConfigure->cropArea.rightOffset = CHECK_ODD(frame->width);    // odd
    inputConfigure->cropArea.upOffset = 0;                              // even
    inputConfigure->cropArea.downOffset = CHECK_ODD(frame->height);     // odd

    // Construct output buffer
    VpcUserRoiOutputConfigure *outputConfigure = &roiConfigureOutput0->outputConfigure;
    outputConfigure->widthStride = ALIGN_UP(frame->width, 128);   // align to 128
    outputConfigure->heightStride = ALIGN_UP(frame->height, 16);  // align to 16
    outputConfigure->bufferSize = outputConfigure->widthStride * outputConfigure->heightStride * YUV_BYTES;  // yuv420sp_UV
    // Construct output buffer
    uint8_t *output0Buffer = nullptr;
    HIAI_StatusT ret = hiai::HIAIMemory::HIAI_DVPP_DMalloc(outputConfigure->bufferSize, (void *&)output0Buffer);
    if (ret != HIAI_OK || output0Buffer == nullptr) {
        HIAI_ENGINE_LOG(APP_ERROR, " HIAI_DVPP_DMalloc of VdecImageResize() error!");
        return HIAI_ERROR;
    }
    outputConfigure->addr = output0Buffer;  // align to 128

    // set crop area:
    outputConfigure->outputArea.leftOffset = 0;  // algin to 16
    outputConfigure->outputArea.rightOffset = CHECK_ODD(frame->width);
    outputConfigure->outputArea.upOffset = 0;
    outputConfigure->outputArea.downOffset = CHECK_ODD(frame->height);

    /* ouput 1: resized image for detect network input */
    inputConfigure = &roiConfigureOutput1->inputConfigure;
    // set map area: the entire picture
    inputConfigure->cropArea = roiConfigureOutput0->inputConfigure.cropArea;  // same as output 0

    // Construct output buffer
    outputConfigure = &roiConfigureOutput1->outputConfigure;
    outputConfigure->widthStride = ALIGN_UP(g_detectInputWidth, 128);  // align to 128
    outputConfigure->heightStride = ALIGN_UP(g_detectInputHeight, 16);  // align to 16
    outputConfigure->bufferSize = outputConfigure->widthStride * outputConfigure->heightStride * YUV_BYTES;  // yuv420sp_UV
    uint8_t *output1Buffer = nullptr;
    ret = hiai::HIAIMemory::HIAI_DVPP_DMalloc(outputConfigure->bufferSize, (void *&)output1Buffer);
    if (ret != HIAI_OK || output1Buffer == nullptr) {
        HIAI_ENGINE_LOG(APP_ERROR, " HIAI_DVPP_DMalloc of VdecImageResize() error!");
        hiai::HIAIMemory::HIAI_DVPP_DFree(roiConfigureOutput0->outputConfigure.addr);
        return HIAI_ERROR;
    }
    outputConfigure->addr = output1Buffer;  // align to 128

    // set crop area:
    outputConfigure->outputArea.leftOffset = 0;  // align to 16
    outputConfigure->outputArea.rightOffset = CHECK_ODD(g_detectInputWidth);
    outputConfigure->outputArea.upOffset = 0;
    outputConfigure->outputArea.downOffset = CHECK_ODD(g_detectInputHeight);

    /* process of VPC */
    dvppapi_ctl_msg dvppApiCtlMsg;
    dvppApiCtlMsg.in = static_cast<void *>(imageConfigure.get());
    dvppApiCtlMsg.in_size = sizeof(VpcUserImageConfigure);

    if (0 != DvppCtl(piDvppApiVpc, DVPP_CTL_VPC_PROC, &dvppApiCtlMsg)) {
        HIAI_ENGINE_LOG(APP_ERROR, " vpc of VdecImageResize() error!");
        hiai::HIAIMemory::HIAI_DVPP_DFree(roiConfigureOutput0->outputConfigure.addr);
        hiai::HIAIMemory::HIAI_DVPP_DFree(roiConfigureOutput1->outputConfigure.addr);
        return HIAI_ERROR;
    }

    /* Get output image */
    while (imageConfigure->roiConfigure != nullptr) {
        ImageInfo img;
        img.format = OUTPUT_YUV420SP_UV;
        img.width = imageConfigure->roiConfigure->outputConfigure.outputArea.rightOffset;
        img.height = imageConfigure->roiConfigure->outputConfigure.outputArea.downOffset;
        img.widthAligned = imageConfigure->roiConfigure->outputConfigure.widthStride;
        img.heightAligned = imageConfigure->roiConfigure->outputConfigure.heightStride;
        img.buf.data = std::shared_ptr<uint8_t>(imageConfigure->roiConfigure->outputConfigure.addr,
                                                [](std::uint8_t *data) { hiai::HIAIMemory::HIAI_DVPP_DFree(data); });
        img.buf.len_of_byte = imageConfigure->roiConfigure->outputConfigure.bufferSize;

        /* update */
        cropImageList.push_back(img);
        imageConfigure->roiConfigure = imageConfigure->roiConfigure->next;
    }

    return HIAI_OK;
}
