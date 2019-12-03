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
#ifndef _DATA_REGISTER_H_
#define _DATA_REGISTER_H_

#include "AppCommon.h"
#include "hiaiengine/data_type.h"
#include "hiaiengine/data_type_reg.h"
#include "hiaiengine/ai_memory.h"

HIAI_REGISTER_DATA_TYPE("ResultInfoT", ResultInfoT);

void GetGraphCtrlInfoSearPtr(void* dataPtr, std::string& structStr, uint8_t*& buffer, uint32_t& bufferSize)
{
    GraphCtrlInfoT *ctrlInfo = static_cast<GraphCtrlInfoT *>(dataPtr);
    structStr  = std::string(static_cast<char *>(dataPtr), sizeof(GraphCtrlInfoT));
    buffer = static_cast<uint8_t *>(ctrlInfo->dataBuff.get());
    bufferSize = ctrlInfo->bufferSize;
}

std::shared_ptr<void> GetGraphCtrlInfoDearPtr(
    const char* ctrlPtr, const uint32_t& ctrlLen,
    const uint8_t* dataPtr, const uint32_t& dataLen)
{
    std::shared_ptr<GraphCtrlInfoT> ctrlInfo = std::make_shared<GraphCtrlInfoT>();
    ctrlInfo->chipId = ((GraphCtrlInfoT*)ctrlPtr)->chipId;
    ctrlInfo->graphId = ((GraphCtrlInfoT*)ctrlPtr)->graphId;
    ctrlInfo->dEngineCount = ((GraphCtrlInfoT*)ctrlPtr)->dEngineCount;
    ctrlInfo->decodeCount = ((GraphCtrlInfoT*)ctrlPtr)->decodeCount;
    ctrlInfo->testType = ((GraphCtrlInfoT*)ctrlPtr)->testType;
    //
    ctrlInfo->bufferSize = ((GraphCtrlInfoT*)ctrlPtr)->bufferSize;
    ctrlInfo->dataBuff.reset((unsigned char*)dataPtr, hiai::HIAIMemory::HIAI_DFree);

    ctrlInfo->width = ((GraphCtrlInfoT *)ctrlPtr)->width;
    ctrlInfo->height = ((GraphCtrlInfoT *)ctrlPtr)->height;
    ctrlInfo->format = ((GraphCtrlInfoT *)ctrlPtr)->format;
    ctrlInfo->level = ((GraphCtrlInfoT *)ctrlPtr)->level;

    return std::static_pointer_cast<void>(ctrlInfo);
}


HIAI_REGISTER_SERIALIZE_FUNC("GraphCtrlInfoT", GraphCtrlInfoT, GetGraphCtrlInfoSearPtr, GetGraphCtrlInfoDearPtr);
HIAI_REGISTER_DATA_TYPE("JpegEncodeInfo", JpegEncodeInfo);


#endif
