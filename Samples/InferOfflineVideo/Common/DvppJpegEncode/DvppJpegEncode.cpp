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
#include "DvppJpegEncode.h"
#include <string.h>

#define CHECK_ODD(NUM) (((NUM) % 2 != 0) ? (NUM) : ((NUM) - 1))
#define CHECK_EVEN(NUM) (((NUM) % 2 == 0) ? (NUM) : ((NUM) - 1))

DvppJpegEncode::DvppJpegEncode()
{
    if (pidvppapi == nullptr) {
        CreateDvppApi(pidvppapi);
        if (pidvppapi == nullptr) {
            HIAI_ENGINE_LOG(HIAI_ERROR, "Failed to CreateDvppApi in  %s line ", __FILE__, __LINE__);
            return;
        }
    }
}

DvppJpegEncode::~DvppJpegEncode()
{
    if (pidvppapi != nullptr) {
        DestroyDvppApi(pidvppapi);
    }
}

HIAI_StatusT DvppJpegEncode::Encode(JpegEncodeIn& jpegInData, JpegEncodeOut& encodedData)
{
    sJpegeIn inData;
    sJpegeOut outData;
    inData.width = CHECK_EVEN(jpegInData.inWidth);
    inData.height = CHECK_EVEN(jpegInData.inHeight);
    inData.heightAligned = jpegInData.inHeight;
    inData.format = jpegInData.format;
    inData.level = jpegInData.level;
    printf("indata is %d %d %d %x\n", jpegInData.inWidth, jpegInData.inHeight, 
        jpegInData.inBufferSize, jpegInData.inBufferPtr.get()); 
    inData.stride = inData.format <= JPGENC_FORMAT_YUYV ? ALIGN_UP(inData.width * NUMBER_2, ALIGN_16) :
        ALIGN_UP(inData.width, ALIGN_16);
    inData.bufSize = inData.format <= JPGENC_FORMAT_YUYV ? 
        ALIGN_UP(inData.stride * inData.heightAligned, PAGE_SIZE) : 
        ALIGN_UP(inData.stride * inData.heightAligned * NUMBER_3 / NUMBER_2, PAGE_SIZE);
    inData.buf = (unsigned char*)ALIGN_UP((uint64_t)jpegInData.inBufferPtr.get(), ALIGN_128);
    
    dvppapi_ctl_msg dvppApiCtlMsg;
    dvppApiCtlMsg.in = static_cast<void*>(&inData);
    dvppApiCtlMsg.in_size = sizeof(sJpegeIn);
    dvppApiCtlMsg.out = static_cast<void*>(&outData);
    dvppApiCtlMsg.out_size = sizeof(sJpegeOut);
    IDVPPAPI* pidvppapi = NULL;
    CreateDvppApi(pidvppapi);
    if (pidvppapi == NULL) {
        printf("can not open dvppapi engine\n");
        return HIAI_ERROR;
    }
    if (0 != DvppCtl(pidvppapi, DVPP_CTL_JPEGE_PROC, &dvppApiCtlMsg)) {
        DestroyDvppApi(pidvppapi);
        printf("dvpp process error \n");
        return HIAI_ERROR;
    }
    encodedData.outBufferPtr = std::shared_ptr<uint8_t>(new uint8_t[outData.jpgSize]);
    encodedData.outBufferSize = outData.jpgSize;
    errno_t ret = memcpy_s(encodedData.outBufferPtr.get(), encodedData.outBufferSize, outData.jpgData, outData.jpgSize);
    if (ret != EOK) {
        printf("memcpy_s in file %s , line %d failed!\n", __FILE__, __LINE__);
        return HIAI_ERROR;
    }
    outData.cbFree();
    DestroyDvppApi(pidvppapi);
    return HIAI_OK;
}

