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

#ifndef ENGINE_IMAGE_TRANS_H
#define ENGINE_IMAGE_TRANS_H

#include "hiaiengine/data_type.h"
#include "hiaiengine/data_type_reg.h"
#include "hiaiengine/status.h"
#include "hiaiengine/ai_memory.h"
#include "Common.h"

using hiai::BatchInfo;
using hiai::IMAGEFORMAT;
using hiai::ImageData;

// 注册EngineTransNew结构体
typedef struct EngineImageTrans {
    std::shared_ptr<uint8_t> trans_buff = nullptr;  // 传输buffer
    uint32_t buffer_size = 0;                       // 传输buffer大小
    uint32_t width = 0;
    uint32_t height = 0;
    std::shared_ptr<uint8_t> trans_buff_extend = nullptr;
    uint32_t buffer_size_extend = 0;
    // std::vector<crop_rect> crop_list;
    // 序列化函数
    template<class Archive>
    void serialize(Archive &ar)
    {
        ar(buffer_size, width, height, buffer_size_extend);
    }
} EngineImageTransT;

static void GetEngineImageTransPtr(void *data_ptr, std::string &struct_str,
                                   uint8_t *&buffer, uint32_t &buffer_size)
{
    EngineImageTrans *engine_trans = (EngineImageTrans *)data_ptr;
    uint8_t *dataPtr = (uint8_t *)engine_trans->trans_buff.get();
    uint32_t dataLen = engine_trans->buffer_size;
    uint8_t *dataPtr_extend = (uint8_t *)engine_trans->trans_buff_extend.get();
    uint32_t dataLen_extend = engine_trans->buffer_size_extend;
    std::shared_ptr<uint8_t> data = ((EngineImageTrans *)data_ptr)->trans_buff;
    std::shared_ptr<uint8_t> data_extend = ((EngineImageTrans *)data_ptr)->trans_buff_extend;
    buffer_size = dataLen + dataLen_extend;
    buffer = (uint8_t *)engine_trans->trans_buff.get();
    engine_trans->trans_buff = nullptr;
    engine_trans->trans_buff_extend = nullptr;

    std::ostringstream outputStr;
    cereal::PortableBinaryOutputArchive archive(outputStr);
    archive((*engine_trans));
    struct_str = outputStr.str();
    // 获取结构体buffer和size
    ((EngineImageTrans *)data_ptr)->trans_buff = data;
    ((EngineImageTrans *)data_ptr)->trans_buff_extend = data_extend;
}

static std::shared_ptr<void> GetEngineImageTransrPtr(const char *ctrlPtr, const uint32_t &ctrlLen,
                                                     const uint8_t *dataPtr, const uint32_t &dataLen)
{
    if (ctrlPtr == nullptr) {
        return nullptr;
    }
    std::shared_ptr<EngineImageTrans> engine_trans_ptr = std::make_shared<EngineImageTrans>();
    std::istringstream inputStream(std::string(ctrlPtr, ctrlLen));
    cereal::PortableBinaryInputArchive archive(inputStream);
    archive((*engine_trans_ptr));
    uint32_t DataLen = engine_trans_ptr->buffer_size;
    uint32_t DataLen_extend = engine_trans_ptr->buffer_size_extend;
    if (dataPtr != nullptr) {
        (engine_trans_ptr->trans_buff).reset((const_cast<uint8_t *>(dataPtr)), hiai::HIAIMemory::HIAI_DFree);
        (engine_trans_ptr->trans_buff_extend).reset((const_cast<uint8_t *>(dataPtr + DataLen)), deleteNothing);
    }
    return std::static_pointer_cast<void>(engine_trans_ptr);
}

#endif
