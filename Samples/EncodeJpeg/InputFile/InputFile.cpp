/* copyright (C) 2019, Huawei Technologies Co., Ltd. All Rights Reserved.
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

#include <vector>
#include <cstdio>
#include <fstream>
#include <cstring>
#include "InputFile.h"
#include "hiaiengine/log.h"
#include "hiaiengine/ai_memory.h"

// define the serialize and deserialize function of YUVImageDataT
HIAI_REGISTER_SERIALIZE_FUNC("YUVImageDataT", YUVImageDataT, GetTransSearPtr, GetTransDearPtr);

/*
 * function: Init the Engine
 * params: config, configuration of the engine
 * params: model_desc, description of model
 * return: HIAI_StatusT, success or not
 */
HIAI_StatusT InputFile::Init(const hiai::AIConfig &config,
    const std::vector<hiai::AIModelDescription> &model_desc)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[InputFile] start init!");
    printf("[InputFile] start init!\n");
    HIAI_StatusT status = HIAI_OK;
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[InputFile] end init!");
    return HIAI_OK;
}

/*
 * function: main function of this engine
 * return: HIAI_StatusT, success or not
 */
HIAI_IMPL_ENGINE_PROCESS("InputFile", InputFile, IF_INPUT_SIZE)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[InputFile] start process!");
    // check the validity of input argument
    if (arg0 == nullptr) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[InputFile] The input arg0 is nullptr");
        return HIAI_ERROR;
    }
    std::shared_ptr<YUVInfoT> inputArg = std::static_pointer_cast<YUVInfoT>(arg0);
    // extract the input file info from input arg
    std::string inputFile = inputArg->fileName;
    inputWidth = inputArg->width;
    inputHeight = inputArg->height;
    format = inputArg->format;
    HIAI_StatusT ret = HIAI_OK;
    printf("Load %s ...\n", inputFile.c_str());
    // check the validity of input file
    char c[PATH_MAX + 1] = {0x00};
    errno_t err = strcpy_s(c, PATH_MAX + 1, inputFile.c_str());
    if (err != EOK) {
        printf("[InputFile] strcpy %s failed!\n", c);
        return HIAI_ERROR;
    }
    char path[PATH_MAX + 1] = {0x00};
    if (realpath(c, path) == NULL) {
        printf("file not exit %s\n", path);
        return HIAI_ERROR;
    }
    FILE* fp = fopen(path, "rb");
    if (fp == NULL) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[InputFile] Open file failed!");
        return HIAI_ERROR;
    }
    // get the size of input file
    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    if (fileSize <= 0) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[InputFile] file is empty!");
        fclose(fp);
        return HIAI_ERROR;
    }
    fseek(fp, 0, SEEK_SET);
    // allocate memory for the data of input yuv image
    uint8_t* buffer = nullptr;
    ret = hiai::HIAIMemory::HIAI_DMalloc(fileSize + 128, (void*&)buffer);
    if (ret != HIAI_OK || buffer == nullptr) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[InputFile] HIAI_DMalloc failed for input file!");
        fclose(fp);
        return HIAI_ERROR;
    }
    if (fileSize < 0) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[InputFile] file is empty!");
        fclose(fp);
        return HIAI_ERROR;
    }
    // put the input yuv image data into buffer
    if (fread(buffer, 1, fileSize, fp) != fileSize) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[InputFile] read data from file error!");
        fclose(fp);
        return HIAI_ERROR;
    }
    fclose(fp);
    // construct the YUVImageDataT data for sending to next engine
    std::shared_ptr<YUVImageDataT> transferData = std::make_shared<YUVImageDataT>();
    transferData->width = inputWidth;
    transferData->height = inputHeight;
    transferData->format = (eEncodeFormat)format;
    transferData->len_of_byte = fileSize + 128;
    transferData->yuvData = std::shared_ptr<uint8_t>(buffer, [](uint8_t* p){hiai::HIAIMemory::HIAI_DFree(p);});
    ret = SendData(0, "YUVImageDataT", std::static_pointer_cast<void>(transferData));
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG("[InputFile] SendData failed!");
        return HIAI_ERROR;
    }
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[InputFile] end process!");
    printf("[InputFile] end process!\n");
    return HIAI_OK;
}

