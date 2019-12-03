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
 
#ifndef VENC_ENGINE_H
#define VENC_ENGINE_H

#include "hiaiengine/engine.h"
#include "dvpp/dvpp_config.h"
#include "dvpp/idvppapi.h"
#include "error_code.h"

#define INPUT_SIZE 1
#define OUTPUT_SIEZE 1
const uint32_t VIDEO_HEIGHT = 1080;
const uint32_t VIDEO_WIDTH  = 1920;

class VencEngine : public hiai::Engine {
public:
    VencEngine() : codingType(0), videoHeight(VIDEO_HEIGHT), videoWidth(VIDEO_WIDTH), vencHandle(0) 
    {

    }
    HIAI_StatusT Init(const hiai::AIConfig& config, const std::vector<hiai::AIModelDescription>& model_desc);
    ~VencEngine() {}
    HIAI_DEFINE_PROCESS(INPUT_SIZE, OUTPUT_SIEZE)

private: 
    static void VencCallBackDumpFile(struct VencOutMsg* vencOutMsg, void* userData); 
    uint32_t codingType;
    uint32_t videoHeight;
    uint32_t videoWidth;
    int32_t vencHandle;
};

#endif

