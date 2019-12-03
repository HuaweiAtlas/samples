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
 
#include "VideoIn.h"
#include "hiaiengine/log.h"
#include "hiaiengine/data_type.h"
#include "hiaiengine/data_type_reg.h"
#include "hiaiengine/ai_memory.h"
#include <memory>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <string>
#include "DataType.h"
#include "Common.h"

HIAI_StatusT VideoIn::Init(const hiai::AIConfig& config, const std::vector<hiai::AIModelDescription>& model_desc)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[VideoIn] start init!");
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[VideoIn] end init!");
    
    return HIAI_OK;
}

HIAI_IMPL_ENGINE_PROCESS("VideoIn", VideoIn, INPUT_SIZE)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[Input INFO] start process!");
    std::shared_ptr<EncodeVideoBlock> inputArg = std::static_pointer_cast<EncodeVideoBlock>(arg0);
    
    char c[PATH_MAX + 1] = {0x00};
    errno_t err = strcpy_s(c, PATH_MAX + 1, inputArg->rawImageFile.c_str());
    if (err != EOK) {
        printf("[Input ERROR] strcpy %s failed!\n", c);
        return HIAI_ERROR;
    }

    char path[PATH_MAX + 1] = {0x00};
    if (realpath(c, path) == NULL) {
        printf("file not exit %s\n", path);
        return HIAI_ERROR;
    }
    FILE* fp = fopen(path, "rb");
    if (fp == NULL) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[VideoIn] Don't Open %s !", inputArg->rawImageFile.c_str());
        return HIAI_ERROR;
    }
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[Input INFO] read %s successfully!", inputArg->rawImageFile.c_str());
    
    fseek(fp, 0L, SEEK_END);
    int fileSize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    char* inBuffer = nullptr;
    int readLen = 0;
    HIAI_StatusT ret = hiai::HIAIMemory::HIAI_DMalloc(fileSize, (void*&)inBuffer);
    if (ret == HIAI_OK && fileSize > 0 && inBuffer != NULL) {
        readLen = fread(inBuffer, 1, fileSize, fp);
    } else {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[VideoIn] HIAI_DMalloc() return error !");
        fclose(fp);
        return HIAI_ERROR;
    }
    fclose(fp);
    
    inputArg->rawImage.data.reset(reinterpret_cast<uint8_t *>(inBuffer), hiai::HIAIMemory::HIAI_DFree);
    inputArg->rawImage.len_of_byte = readLen;
    
    if (HIAI_OK != hiai::Engine::SendData(0, "EncodeVideoBlock",
                                          std::static_pointer_cast<void>(inputArg))) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[Input INFO] SendData() error!");
        return HIAI_ERROR;
    }

    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[Input INFO] send data successfully, frameId = %d, image size = %d", inputArg->frameId, readLen);
    return HIAI_OK;
}




























