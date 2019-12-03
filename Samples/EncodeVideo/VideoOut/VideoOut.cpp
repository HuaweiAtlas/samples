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
 
#include "VideoOut.h"
#include "hiaiengine/log.h"
#include "DataType.h"
#include "Common.h"

std::string g_vencOutFileName("venc.bin");
std::shared_ptr<FILE> g_vencOutFile(nullptr);

HIAI_StatusT VideoOut::Init(const hiai::AIConfig& config, const std::vector<hiai::AIModelDescription>& model_desc)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[VideoOut] start init!");

    char c[PATH_MAX + 1] = {0x00};
    errno_t err = strcpy_s(c, PATH_MAX + 1, g_vencOutFileName.c_str());
    if (err != EOK) {
        printf("[VideoEncode ERROR] strcpy %s failed!\n", c);
        return HIAI_ERROR;
    }
    char path[PATH_MAX + 1] = {0x00};
    if (realpath(c, path) == NULL) {
        printf("Begin writing file %s...\n", path);
    } else {
        printf("File %s already exists, overwriting...\n", path);
    }
    g_vencOutFile.reset(fopen(path, "wb"), fclose);
    if (g_vencOutFile.get() == NULL) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[VideoOut] open out file failed");
        return HIAI_ERROR;
    }
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[VideoOut] end init!");
    
    return HIAI_OK;
}
/**
 * @ingroup hiaiengine
 * @brief HIAI_DEFINE_PROCESS : implementaion of the engine
 * @[in]: engine name and the number of input
 */
HIAI_IMPL_ENGINE_PROCESS("VideoOut", VideoOut, INPUT_SIZE)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[VideoOut INFO] start");
    std::shared_ptr<EncodeVideoBlock> inputArg = std::static_pointer_cast<EncodeVideoBlock>(arg0);

    if ((g_vencOutFile.get() == nullptr) || (inputArg->encodedVideoFrame.data.get() == NULL)) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[VideoOut INFO] g_vencOutFile is NULL");
        return HIAI_ERROR;
    }
    fwrite(inputArg->encodedVideoFrame.data.get(), 1, inputArg->encodedVideoFrame.len_of_byte, g_vencOutFile.get());
    fflush(g_vencOutFile.get());

    std::shared_ptr<std::string> sendDataPtr(new std::string());
    if (HIAI_OK != hiai::Engine::SendData(0, "string", std::static_pointer_cast<void>(sendDataPtr))) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[VideoOut INFO] SendData() error!");
        return HIAI_ERROR;
    }
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[VideoOut INFO] Save franeID: %d, size: %d", inputArg->frameId, inputArg->encodedVideoFrame.len_of_byte);
    
    return HIAI_OK;
}
