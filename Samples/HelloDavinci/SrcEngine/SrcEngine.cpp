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

#include "SrcEngine.h"
#include <hiaiengine/log.h>
#include <stdio.h>

HIAI_REGISTER_DATA_TYPE("input_data_st", input_data_st);

HIAI_StatusT SrcEngine::Init(const hiai::AIConfig& config, const  std::vector<hiai::AIModelDescription>& model_desc)
{
        if (NULL == data_config_)
        {
            data_config_ = std::make_shared<comm_context_st>();
        }

        for (int index = 0; index < config.items_size(); ++index)
        {
            const ::hiai::AIConfigItem& item = config.items(index);
            std::string name = item.name();
            if(name == "type"){
                std::string value = item.value();
                data_config_->type = atoi(value.data());
                std::cout << "[SrcEngine] data type - " << data_config_->type<<std::endl;
            }
        }
        return HIAI_OK;
}

HIAI_IMPL_ENGINE_PROCESS("SrcEngine", SrcEngine, INPUT_SIZE)
{

    std::shared_ptr<input_data_st> data_to_send = std::make_shared<input_data_st>();
    data_to_send->input_info = *data_config_;

    int hiai_ret = SendData(0, "input_data_st", std::static_pointer_cast<void>(data_to_send));
    if (hiai_ret != HIAI_OK)
    {
        HIAI_ENGINE_LOG("[SrcEngine]SendData fail!ret = %d", hiai_ret);
    }

    return HIAI_OK;
}
