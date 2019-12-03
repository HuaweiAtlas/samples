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

#ifndef DecodeEngine_ENGINE_H
#define DecodeEngine_ENGINE_H
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <map>
#include <queue>
#include <hiaiengine/ai_model_manager.h>
#include "hiaiengine/engine.h"
#include "hiaiengine/data_type_reg.h"
#include "hiaiengine/api.h"
#include "dvpp/dvpp_config.h"
#include "dvpp/Vpc.h"
#include "dvpp/Vdec.h"
#include "dvpp/idvppapi.h"

#include "common_data_type.h"
#include "error_code.h"

#define DECODE_INPUT_SIZE  1
#define DECODE_OUTPUT_SIZE 3

#define CHECK_ODD(NUM)  ((((NUM) % (2)) != (0)) ? (NUM) : ((NUM) - (1)))
#define CHECK_EVEN(NUM) ((((NUM) % (2)) == (0)) ? (NUM) : ((NUM) - (1)))

/* Read the input size of model from om file, for resize the input image in the DecodeEngine */
extern int g_detectInputWidth;
extern int g_detectInputHeight;

// define struct that used to send frame and frame ID from get_vdec_result() to host process
typedef struct {
    StreamInfo info;
    std::vector<ImageInfo> cropImageList;
} FRAME_BUFFER;

// define an engine
class DecodeEngine : public Engine {
public:
    DecodeEngine()
    {
        frameId = 0;

        frameInfo.mode = MODE_NORMAL;
        frameInfo.format = FORMAT_JPG;
        frameInfo.channelId = 0;
        frameInfo.frameId = 0;
        frameInfo.isEOS = 0;
    }

    ~DecodeEngine();

    static void VdecResultCallback(FRAME *frame, void *hiai_data);
    static void VdecErrorCallback(VDECERR *vdecErr);

    HIAI_StatusT Init(const AIConfig &config, const std::vector<AIModelDescription> &model_desc);

    HIAI_StatusT JpgImageDecode(uint8_t *jpgImageDataBuf, int jpgImageDataLength, ImageInfo &cropRawImage,
                                ImageInfo &cropDetectImage);
    HIAI_StatusT JpgImageResize(JpegdOut &jpegdOutData, std::vector<ImageInfo> &cropImageList);
    HIAI_StatusT DecodeH26xVideo(std::shared_ptr<StreamRawData> streamRawData);
    HIAI_StatusT VdecImageResize(FRAME *frame, std::vector<ImageInfo> &cropImageList);

    /**
    * @ingroup hiaiengine
 */
    HIAI_DEFINE_PROCESS(DECODE_INPUT_SIZE, DECODE_OUTPUT_SIZE);

private:
    /* define DVPP object */
    IDVPPAPI *piDvppApiVdec = NULL;
    IDVPPAPI *piDvppApiVpc = NULL;
    IDVPPAPI *piDvppApiJpegD = NULL;

    /* transfer data between the callback functiong and main process */
    queue<FRAME_BUFFER> frameQueue;

    /* for decode */
    StreamInfo frameInfo;
    uint64_t frameId; /* frame id, in video stream channel */
};
#endif
