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
#include "common_data_type.h"
#include "hiaiengine/ai_memory.h"
#include "hiaiengine/log.h"
#include <cstdio>

HIAI_REGISTER_SERIALIZE_FUNC("EngineTransNewT", EngineTransNewT, GetTransSearPtr, GetTransDearPtr);
HIAI_REGISTER_SERIALIZE_FUNC("StreamRawData", StreamRawData, GetStreamRawDataSearPtr, GetStreamRawDataDearPtr);

HIAI_StatusT SrcEngine::Init(const hiai::AIConfig &config, const std::vector<hiai::AIModelDescription> &model_desc)
{
    printf("[SrcEngine] start init!\n");

    for (int index = 0; index < config.items_size(); ++index) {
        const ::hiai::AIConfigItem &item = config.items(index);
        std::string name = item.name();
        if (name == "channel_id") {
            std::string value = item.value();
            dataConfig.channelId = atoi(value.data());
            // std::cout << "[SrcEngine] channel - " << dataConfig.channel_id << std::endl;
        } else if (name == "format") {
            std::string format_value = item.value();
            if (format_value == "h264") {
                dataConfig.format = H264;
            } else if (format_value == "h265") {
                dataConfig.format = H265;
            } else {
                HIAI_ENGINE_LOG("[SrcEngine] Unsupported format!");
                std::cerr << "Unsupported format!" << std::endl;
                return HIAI_ERROR;
            }
        }
    }
    printf("[SrcEngine] end init!\n");
    return HIAI_OK;
}

HIAI_IMPL_ENGINE_PROCESS("SrcEngine", SrcEngine, SRC_INPUT_SIZE)
{
    HIAI_StatusT ret = HIAI_OK;
    std::shared_ptr<std::string> inputFile = std::static_pointer_cast<std::string>(arg0);
    printf("[SrcEngine] open file %s\n", inputFile->c_str());
    FILE *fp;
    fp = fopen(inputFile->c_str(), "rb");
    if (NULL == fp) {
        printf("[SrcEngine] file open error\n");
        return HIAI_ERROR;
    }
    fseek(fp, 0L, SEEK_END);
    int file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    if (file_size <= 0) {
        printf("[SrcEngine] read empty file\n");
        fclose(fp);
        fp = NULL;
        return HIAI_ERROR;
    }
    unsigned char *in_buffer = NULL;
    ret = hiai::HIAIMemory::HIAI_DMalloc(file_size, (void *&)in_buffer,
                                         hiai::MALLOC_DEFAULT_TIME_OUT, hiai::HIAI_MEMORY_ATTR_MANUAL_FREE);
    if (ret != HIAI_OK || in_buffer == NULL) {
        printf("[SrcEngine] HIAI_DMalloc buffer faild\n");
        fclose(fp);
        fp = NULL;
        return HIAI_ERROR;
    }
    int read_len = fread(in_buffer, 1, file_size, fp);
    fclose(fp);
    fp = NULL;
    std::shared_ptr<StreamRawData> data_to_send = std::make_shared<StreamRawData>();
    data_to_send->info = dataConfig;
    data_to_send->info.isEOS = 0;
    data_to_send->buf.bufferSize = read_len;
    data_to_send->buf.transBuff = std::shared_ptr<uint8_t>(in_buffer, hiai::HIAIMemory::HIAI_DFree);
    int hiai_ret = SendData(0, "StreamRawData", std::static_pointer_cast<void>(data_to_send));
    if (hiai_ret != HIAI_OK) {
        printf("[SrcEngine] SendData fail!ret = %d\n", hiai_ret);
    }

    std::shared_ptr<StreamRawData> eos_to_send = std::make_shared<StreamRawData>();
    eos_to_send->info = dataConfig;
    eos_to_send->info.isEOS = 1;
    hiai_ret = SendData(0, "StreamRawData", std::static_pointer_cast<void>(eos_to_send));
    if (hiai_ret != HIAI_OK) {
        printf("[SrcEngine] SendData fail!ret = %d\n", hiai_ret);
    }

    return HIAI_OK;
}
