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
#ifndef PNGDRESIZE_H
#define PNGDRESIZE_H
#include <string>
#include <vector>
#include "dvpp/Vpc.h"
#include "dvpp/idvppapi.h"
#include "hiaiengine/data_type.h"
#include "hiaiengine/engine.h"
#include "hiaiengine/multitype_queue.h"
#include "AppCommon.h"
#include "EngineImageTrans.h"
#include "CropResize.h"
#define PNGD_INPUT_SIZE  1
#define PNGD_OUTPUT_SIZE 1

using hiai::Engine;
using namespace std;
using namespace hiai;
class PNGDResize : public Engine {
public:
    PNGDResize():input_que_(PNGD_INPUT_SIZE)
    {
    }
    HIAI_StatusT Init(const hiai::AIConfig &config, const std::vector<hiai::AIModelDescription> &model_desc);

    /**
    * @ingroup hiaiengine
    * @brief HIAI_DEFINE_PROCESS : reload Engine Process
    * @[in]: define the number of input and output
 */
    HIAI_DEFINE_PROCESS(PNGD_INPUT_SIZE, PNGD_OUTPUT_SIZE)

    ~PNGDResize();

private:
    // Private implementation a member variable, which is used to cache the input queue
    hiai::MultiTypeQueue input_que_;
    HIAI_StatusT SendDataToDst(const shared_ptr<CropResizeOutputImage> cropResizeOutputImage);
    HIAI_StatusT ResizeImage(const std::shared_ptr<DecodeOutputImage> decodeOutputImage,
                             shared_ptr<CropResizeOutputImage> cropResizeOutputImage,
                             const float resizeFactorW, const float resizeFactorH);
};

#endif
