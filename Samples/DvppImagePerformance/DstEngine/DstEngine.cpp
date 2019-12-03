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
#include <hiaiengine/log.h>
#include <hiaiengine/ai_types.h>
#include "hiaiengine/ai_model_parser.h"
#include <vector>
#include <unistd.h>
#include <thread>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <sys/stat.h>
#include <sstream>
#include <stdio.h>
#include <sys/time.h>

using namespace std;

HIAI_StatusT DstEngine::Init(const hiai::AIConfig& config, const  std::vector<hiai::AIModelDescription>& model_desc)
{
    return HIAI_OK;
}

HIAI_IMPL_ENGINE_PROCESS("DstEngine", DstEngine, INPUT_SIZE)
{    
    shared_ptr<ResultInfoT> resultInfo = std::static_pointer_cast<ResultInfoT>(arg0);
    
    double costMs = 0;
    costMs = 1000.0*(resultInfo->endTime.tv_sec-resultInfo->startTime.tv_sec) + \
        (resultInfo->endTime.tv_usec-resultInfo->startTime.tv_usec)/1000.0;
    double fps = resultInfo->decodeCount*1000.0/costMs;

    cout << "chip:" << resultInfo->chipId \
    << "\tgraph:" << resultInfo->graphId \
    << "\tthread:" << resultInfo->dEngineCount \
    << "\tcount:" << resultInfo->decodeCount \
    << "\tfailed:" << resultInfo->failedCount \
    << "\tcost:" << costMs << "ms" \
    << "\tfps:" << fps << endl;

    std::shared_ptr<std::string> dstData(new std::string);
    int hiai_ret = SendData(0, "string", std::static_pointer_cast<void>(dstData));
    if (hiai_ret != HIAI_OK) {
        HIAI_ENGINE_LOG("[DstEngine]SendData fail!ret = %d", hiai_ret);
    }
    return HIAI_OK;
}
