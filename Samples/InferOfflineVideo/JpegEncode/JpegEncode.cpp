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
#include "stream_data.h"

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

HIAI_IMPL_ENGINE_PROCESS("JpegEncode", JpegEncode, JC_INPUT_SIZE)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[JpegEncode] start process!");
    if (arg0 == nullptr) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[JpegEncode]  The input arg0 is nullptr");
        return HIAI_ERROR;
    }
    std::shared_ptr<DeviceStreamData> deviceStreamData = std::static_pointer_cast<DeviceStreamData>(arg0);
    HIAI_StatusT ret;
    // the stream is end
    if (deviceStreamData->info.isEOS) {
        ret = SendData(0, "DeviceStreamData", deviceStreamData);
        HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[JpegEncode]  The stream is end!");
        return HIAI_OK;
    }
    JpegEncodeIn inData;
    JpegEncodeOut outData;
    inData.inWidth = deviceStreamData->imgOrigin.width;
    inData.inHeight = deviceStreamData->imgOrigin.height;
    inData.inBufferSize = deviceStreamData->imgOrigin.buf.len_of_byte;
    inData.inBufferPtr = deviceStreamData->imgOrigin.buf.data;
    //    JPGENC_FORMAT_UYVY = 0x0,
    //    JPGENC_FORMAT_VYUY = 0x1,
    //    JPGENC_FORMAT_YVYU = 0x2,
    //    JPGENC_FORMAT_YUYV = 0x3,
    //    JPGENC_FORMAT_NV12 = 0x10,
    //    JPGENC_FORMAT_NV21 = 0x11,
    inData.format = JPGENC_FORMAT_NV12;
    inData.level = 100;
    ret = dvppJpegEapi->Encode(inData, outData);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[JpegEncode]  failed to encode jpeg!");
        return HIAI_ERROR;
    }

    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[JpegEncode]  in out buffer size is %d vs %d, %x\n", inData.inBufferSize,
            outData.outBufferSize, outData.outBufferPtr.get());

    deviceStreamData->imgOrigin.buf.len_of_byte = outData.outBufferSize;
    deviceStreamData->imgOrigin.buf.data = outData.outBufferPtr;

    ret = SendData(0, "DeviceStreamData", deviceStreamData);
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[JpegEncode] end process!");
    return HIAI_OK;
}

