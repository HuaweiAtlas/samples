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

#ifndef TRANS_SEAR__H
#define TRANS_SEAR__H

#include <unistd.h>
#include <sys/stat.h>
#include "hiaiengine/data_type.h"
#include "hiaiengine/data_type_reg.h"
#include "hiaiengine/status.h"
#include "hiaiengine/ai_memory.h"
#include "AppCommon.h"
typedef struct GraphCtrlInfo {
    uint32_t bufferSize;
    std::shared_ptr<uint8_t> dataBuff;
} GraphCtrlInfoT;

static void GetTransSearPtr(void *dataPtr, std::string &structStr, uint8_t *&buffer, uint32_t &bufferSize)
{
    GraphCtrlInfoT *ctrlInfo = (GraphCtrlInfoT *)dataPtr;
    structStr = std::string((char *)dataPtr, sizeof(GraphCtrlInfoT));
    buffer = (uint8_t *)ctrlInfo->dataBuff.get();
    bufferSize = ctrlInfo->bufferSize;
}

static std::shared_ptr<void> GetTransDearPtr(const char *ctrlPtr,
                                             const uint32_t &ctrllen, const uint8_t *dataPtr, const uint32_t &dataLen)
{
    std::shared_ptr<GraphCtrlInfoT> ctrlInfo = std::make_shared<GraphCtrlInfoT>();
    ctrlInfo->bufferSize = ((GraphCtrlInfoT *)ctrlPtr)->bufferSize;
    ctrlInfo->dataBuff.reset((unsigned char *)dataPtr, hiai::HIAIMemory::HIAI_DFree);
    return std::static_pointer_cast<void>(ctrlInfo);
}

#endif
