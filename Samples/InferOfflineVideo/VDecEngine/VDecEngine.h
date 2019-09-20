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
#ifndef ATLASSAMPLES_VDECENGINE_H
#define ATLASSAMPLES_VDECENGINE_H

// #include "app_common.h"
#include "common_data_type.h"
#include "dvpp/idvppapi.h"
#include "dvpp/vdec_hiai.h"
#include "hiaiengine/engine.h"

#define VD_INPUT_SIZE 1
#define VD_OUTPUT_SIZE 1

class VDecEngine : public hiai::Engine {
public:
    VDecEngine() {}
    ~VDecEngine();
    HIAI_StatusT Init(const hiai::AIConfig& config,
        const std::vector<hiai::AIModelDescription>& model_desc);

    HIAI_DEFINE_PROCESS(VD_INPUT_SIZE, VD_OUTPUT_SIZE)

    HIAI_StatusT Hfbc2YuvOld(FRAME* frame, vpc_in_msg& vpcInMsg);

private:
    static void frameCallback(FRAME* frame, void* hiai_data);

    IDVPPAPI* pVdecHandle = NULL;
    IDVPPAPI* pDvppHandle = NULL;
    vdec_in_msg vdecInMsg;
    StreamInfo inputInfo;
    uint64_t frameId = 0;
    time_pair stamps;
};

#endif //ATLASSAMPLES_VDECENGINE_H
