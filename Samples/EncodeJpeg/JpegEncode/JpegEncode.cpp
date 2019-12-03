/* copyright (C) 2019, Huawei Technologies Co., Ltd. All Rights Reserved.
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

#include <vector>
#include <cstdio>
#include <fstream>
#include <cstring>
#include "JpegEncode.h"
#include "DvppJpegEncode.h"
#include "hiaiengine/log.h"
#include "hiaiengine/ai_memory.h"

/*
 * function: Init the Engine
 * params: config, configuration of the engine
 * params: model_desc, description of model
 * return: HIAI_StatusT, success or not
 */
HIAI_StatusT JpegEncode::Init(const hiai::AIConfig &config,
    const std::vector<hiai::AIModelDescription> &model_desc)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[JpegEncode] start init!");
    printf("[JpegEncode] start init\n");
    if (dvppJpegEapi == nullptr) {
        dvppJpegEapi = std::make_shared<DvppJpegEncode>();
    }
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[JpegEncode] end init!");
    printf("[JpegEncode] end init!\n");
    return HIAI_OK;
}

/*
 * function: main function of this engine
 * return: HIAI_StatusT, success or not
 */
HIAI_IMPL_ENGINE_PROCESS("JpegEncode", JpegEncode, JC_INPUT_SIZE)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[JpegEncode] start process!");
    printf("[JpegEncode] start process!\n");
    // check the validity of arg
    if (arg0 == nullptr) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "The input arg0 is nullptr");
        printf("The input arg0 is nullptr\n");
        return HIAI_ERROR;
    }
    
    std::shared_ptr<YUVImageDataT> transferData = std::static_pointer_cast<YUVImageDataT>(arg0);
    HIAI_StatusT ret = HIAI_OK;
    // extract the Jpeg Encode data from the input arg
    JpegEncodeIn inData;
    JpegEncodeOut outData;
    inData.inWidth = transferData->width;
    inData.inHeight = transferData->height;
    inData.inBufferSize = transferData->len_of_byte;
    inData.inBufferPtr = transferData->yuvData;
    inData.format = transferData->format;
    inData.alignMethod = 1;
    inData.level = 100;
    // encode the yuv image into jpeg
    ret = dvppJpegEapi->Encode(inData, outData);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "failed to encode jpeg!");
        printf("failed to encode jpeg!\n");
        return HIAI_ERROR;
    }
    // construct the jpeg data sending to next engine
    std::shared_ptr<hiai::RawDataBuffer> rawData = std::make_shared<hiai::RawDataBuffer>();
    rawData->len_of_byte = outData.outBufferSize;
    rawData->data = outData.outBufferPtr;
    ret = SendData(0, "RawDataBuffer", std::static_pointer_cast<void>(rawData));
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[InputFile] end process!");
    printf("[InputFile] end process!\n");
    return HIAI_OK;
}

