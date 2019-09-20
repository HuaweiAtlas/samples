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

#ifndef APP_COMMON_H
#define APP_COMMON_H

#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include "hiaiengine/data_type.h"
#include "hiaiengine/data_type_reg.h"

using namespace hiai;

typedef struct {
    uint32_t type;
} comm_context_st;
template<class Archive>
void serialize(Archive &ar, comm_context_st &data)
{
    ar(data.type);
}

typedef struct input_data {
    comm_context_st input_info;
} input_data_st;
template<class Archive>
void serialize(Archive &ar, input_data_st &data)
{
    ar(data.input_info);
}

typedef struct Output {
    int32_t size;
    std::shared_ptr<u_int8_t> data;
} OutputT;
template<class Archive>
void serialize(Archive &ar, OutputT &data)
{
    ar(data.size);
    if (data.size > 0 && data.data.get() == nullptr) {
        data.data.reset(new u_int8_t[data.size]);
    }

    ar(cereal::binary_data(data.data.get(), data.size * sizeof(u_int8_t)));
}

typedef struct result_output {
    comm_context_st input_info;
    OutputT result_data;
} result_output_st;
template<class Archive>
void serialize(Archive &ar, result_output_st &data)
{
    ar(data.input_info, data.result_data);
}

#endif
