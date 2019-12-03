/**
 * ============================================================================
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Atlas Sample
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
#include "SaveFile.h"

using namespace std;

/*
 * function: Init the Engine
 * params: config, configuration of the engine
 * params: model_desc, description of model
 * return: HIAI_StatusT, success or not
 */
HIAI_StatusT SaveFile::Init(const hiai::AIConfig &config,
                            const std::vector<hiai::AIModelDescription> &model_desc)
{
    return HIAI_OK;
}

/*
 * function: main function of this engine
 * return: HIAI_StatusT, success or not
 */
HIAI_IMPL_ENGINE_PROCESS("SaveFile", SaveFile, INPUT_SIZE)
{
    HIAI_ENGINE_LOG(HIAI_INFO, "[SaveFile] start process!");
    HIAI_StatusT ret = HIAI_OK;
    // check the validity of arg
    if (arg0 == nullptr) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "The input arg0 is nullptr");
        printf("The input arg0 is nullptr\n");
        return HIAI_ERROR;
    }
    std::shared_ptr<hiai::RawDataBuffer> inputArg = std::static_pointer_cast<hiai::RawDataBuffer>(arg0);
    // check the validity of output file, and write the jpeg data into the jpg file
    void *ptr = (void *)(inputArg->data.get());
    FILE *fp = fopen("./res.jpg", "wb");
    if (NULL == fp) {
        HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[SaveFile] Save file engine: open file fail!");
        return HIAI_ERROR;
    } else {
        fwrite(ptr, 1, inputArg->len_of_byte, fp);
        fflush(fp);
        fclose(fp);
    }
    printf("[SaveFile] result save success!\n");

    std::shared_ptr<std::string> res(new std::string("end"));
    ret = SendData(0, "string", std::static_pointer_cast<void>(res));
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[SaveFile] end process!");
    return ret;
}

/**
 * @brief deconstructor
 */
SaveFile::~SaveFile()
{
}
