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

#include <memory>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include "SaveFile.h"
#include "hiaiengine/log.h"
#include "hiaiengine/data_type.h"
#include "hiaiengine/data_type_reg.h"

using namespace std;
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

HIAI_IMPL_ENGINE_PROCESS("SaveFile", SaveFile, INPUT_SIZE)
{
    HIAI_ENGINE_LOG(HIAI_INFO, "[SaveFile] start process!");
    HIAI_StatusT ret = HIAI_OK;
    std::shared_ptr<hiai::RawDataBuffer> inputArg = std::static_pointer_cast<hiai::RawDataBuffer>(arg0);

    void *ptr = (void *)(inputArg->data.get());
    FILE *fp = fopen("./res.yuv", "wb");
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

SaveFile::~SaveFile()
{
}
