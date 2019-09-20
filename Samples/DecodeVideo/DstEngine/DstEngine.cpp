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

#include "DstEngine.h"
#include "common_data_type.h"
#include <sys/stat.h>

const uint32_t OUT_PATH_MAX = 128;
using Stat = struct stat;

void MkdirP(const std::string& outdir)
{
    Stat st;
    if (stat(outdir.c_str(), &st) != 0) {
        int dir_err = mkdir(outdir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (dir_err == -1) {
            printf("Error creating directory!\n");
            exit(1);
        }
    }
}

HIAI_StatusT DstEngine::Init(const hiai::AIConfig& config, const std::vector<hiai::AIModelDescription>& model_desc)
{
    printf("[DstEngine] start init!\n");
    outCnt = 0;
    printf("[DstEngine] end init!\n");
    return HIAI_OK;
}

HIAI_IMPL_ENGINE_PROCESS("DstEngine", DstEngine, DST_INPUT_SIZE)
{
    const char* outdir = "./result_files";
    MkdirP(outdir);

    std::shared_ptr<StreamRawData> result = std::static_pointer_cast<StreamRawData>(arg0);
    if (nullptr == result) {
        printf("[DstEngine] result is nullptr\n");
        return HIAI_ERROR;
    }

    if (result->info.isEOS) {
        std::shared_ptr<std::string> dst_data(new std::string);
        int hiai_ret = SendData(0, "string", std::static_pointer_cast<void>(dst_data));
        if (hiai_ret != HIAI_OK) {
            printf("[DstEngine] SendData fail!ret = %d\n", hiai_ret);
            return HIAI_ERROR;
        }
    }

    if (result->buf.bufferSize <= 0) {
        printf("[DstEngine] result_data is empty!\n");
        return HIAI_ERROR;
    }

    std::string format;
    if (result->info.format == H264) {
        format = "h264";
    } else {
        format = "h265";
    }

    char path[OUT_PATH_MAX];
    int ret = sprintf_s(path, OUT_PATH_MAX, "%s/h_%d_w_%d_channel%d_result_for_%s_%06d.yuv", outdir,
        result->info.height,
        result->info.width,
        result->info.channelId,
        format.c_str(),
        outCnt++);
    if (ret <= 0) {
        printf("sprintf_s failed\n");
        return HIAI_ERROR;
    }

    FILE* fpOut = fopen(path, "wb+");
    if (fpOut != NULL) {
        printf("[DstEngine] Wirte decoded file %s\n", path);
        fwrite(result->buf.transBuff.get(), 1, result->buf.bufferSize, fpOut);
        fflush(fpOut);
        fclose(fpOut);
        fpOut = NULL;
    } else {
        printf("[DstEngine] cannot open file to wrire\n");
        return HIAI_ERROR;
    }
    return HIAI_OK;
}
