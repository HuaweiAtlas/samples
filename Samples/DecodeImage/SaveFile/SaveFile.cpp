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
#include <fstream>
#include <memory>
#include <sstream>
#include <cstdio>
#include "SaveFile.h"
#include "hiaiengine/data_type.h"
#include "hiaiengine/data_type_reg.h"
#include "hiaiengine/log.h"
#include <string>

using namespace std;

const string RESULT_FOLDER = "result_files/";
const string FILE_PRE_FIX = "result_";

/**
* @ingroup hiaiengine
* @brief HIAI_DEFINE_PROCESS : implementaion of the engine
* @[in]: engine name and the number of input
 */
HIAI_StatusT SaveFile::Init(const hiai::AIConfig &config,
                            const std::vector<hiai::AIModelDescription> &model_desc)
{
    return HIAI_OK;
}

HIAI_IMPL_ENGINE_PROCESS("SaveFile", SaveFile, SAVE_INPUT_SIZE)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[SaveFile] start process!");
    printf("[SaveFile] start process!\n");
    HIAI_StatusT ret = HIAI_OK;

    std::shared_ptr<EngineImageTransT> inputArg = std::static_pointer_cast<EngineImageTransT>(arg0);

    void *ptr = (void *)(inputArg->trans_buff.get());

    if (HIAI_OK != CreateFolder(RESULT_FOLDER, PERMISSION)) {
        return HIAI_ERROR;
    }

    string resultFile = RESULT_FOLDER + FILE_PRE_FIX + to_string(getCurentTime()) + "_h_" + to_string(inputArg->height) + "_w_" + to_string(inputArg->width) + ".yuv";
    char c[PATH_MAX + 1] = { 0x00 };
    errno_t err = strcpy_s(c, PATH_MAX + 1, resultFile.c_str());
    if (err != EOK) {
        printf("[SaveFile] strcpy %s failed!\n", c);
        return HIAI_ERROR;
    }
    char path[PATH_MAX + 1] = { 0x00 };
    if (realpath(c, path) == NULL) {
        printf("Begin writing file %s...\n", path);
    }
    FILE *fp = fopen(path, "wb");
    if (NULL == fp) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[INFO] Save file engine: open file fail!");
        return HIAI_ERROR;
    } else {
        fwrite(ptr, 1, inputArg->buffer_size, fp);
        fflush(fp);
        fclose(fp);
        printf("[SaveFile] Save file successfully!\n");
    }

    std::shared_ptr<std::string> res(new std::string("end"));
    ret = SendData(0, "string", std::static_pointer_cast<void>(res));
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[info] [SaveFile] end process!");
    printf("[SaveFile] end process!\n");
    return ret;
}

SaveFile::~SaveFile()
{
}
