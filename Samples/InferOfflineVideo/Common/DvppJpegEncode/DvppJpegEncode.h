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
#ifndef DVPPJPEGENCODE_H
#define DVPPJPEGENCODE_H

#include "dvpp/idvppapi.h"
#include "dvpp/dvpp_config.h"
#include "hiaiengine/ai_memory.h"
#include "hiaiengine/c_graph.h"
#include "hiaiengine/log.h"

const int ALIGN_16 = 16;
const int ALIGN_128 = 128;
const int NUMBER_2 = 2;
const int NUMBER_3 = 3;

struct JpegEncodeIn {
    uint32_t inWidth;
    uint32_t inHeight;
    uint32_t inBufferSize;
    eEncodeFormat format;
    uint32_t level;
    std::shared_ptr<uint8_t> inBufferPtr;
};

struct JpegEncodeOut {
    uint32_t outBufferSize;
    std::shared_ptr<uint8_t> outBufferPtr;
};

class DvppJpegEncode {
public:

    DvppJpegEncode();

    ~DvppJpegEncode();

    HIAI_StatusT Encode(JpegEncodeIn& jpegInData, JpegEncodeOut& encodedData);

private:
    IDVPPAPI* pidvppapi = nullptr;
};

#endif
