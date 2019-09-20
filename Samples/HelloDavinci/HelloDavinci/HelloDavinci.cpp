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

#include "HelloDavinci.h"
#include "hiaiengine/log.h"
#include "hiaiengine/data_type_reg.h"
#include <stdio.h>
#include <string.h>

HIAI_REGISTER_DATA_TYPE("input_data_st", input_data_st);
HIAI_REGISTER_DATA_TYPE("result_output_st", result_output_st);

HIAI_StatusT HelloDavinci::Init(const AIConfig &config, const std::vector<AIModelDescription> &model_desc)
{
    return HIAI_OK;
}

HelloDavinci::~HelloDavinci(){}

HIAI_IMPL_ENGINE_PROCESS("HelloDavinci", HelloDavinci, HELLODAVINCI_INPUT_SIZE)
{
    std::shared_ptr<input_data_st> input_arg = std::static_pointer_cast<input_data_st>(arg0);
    int hiai_ret;

    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[SrcEngine] Receive data from last engine: %d", input_arg->input_info.type);
    string str = "This message is from HelloDavinci\n";
    char *in_buffer = new char[str.length() + 1];
    for (int i = 0; i < str.length(); i++) {
        in_buffer[i] = str[i];
    }
    in_buffer[str.length()] = '\0';

    std::shared_ptr<result_output_st> out = std::make_shared<result_output_st>();
    out->result_data.size = str.length() + 1;
    out->result_data.data = std::shared_ptr<uint8_t>(reinterpret_cast<uint8_t *>(in_buffer), [](uint8_t *p) { delete[] p; });

    hiai_ret = SendData(0, "result_output_st", std::static_pointer_cast<void>(out));
    if (HIAI_OK != hiai_ret) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "send_data failed!hiai_ret = %d", hiai_ret);
        return hiai_ret;
    }

    return HIAI_OK;
}
