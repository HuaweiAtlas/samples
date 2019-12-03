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

#ifndef ENGINE_TRANS_NEW_H
#define ENGINE_TRANS_NEW_H

#include "hiaiengine/data_type_reg.h"
#include <memory>

struct EngineTransNewT {
    std::shared_ptr<uint8_t> transBuff;
    uint32_t bufferSize;  // buffer��С
};
/**
* @ingroup hiaiengine
* @brief GetTransSearPtr, ���л�Trans����
* @param [in] : dataPtr �ṹ��ָ��
* @param [out]��structStr �ṹ��buffer
* @param [out]��buffer �ṹ������ָ��buffer
* @param [out]��dataSize �ṹ�����ݴ�С
 */
inline void GetTransSearPtr(void *dataPtr, std::string &structStr, uint8_t *&buffer, uint32_t &bufferSize)
{
    EngineTransNewT *engineTrans = static_cast<EngineTransNewT *>(dataPtr);
    structStr = std::string(reinterpret_cast<const char *>(dataPtr), sizeof(EngineTransNewT));
    buffer = reinterpret_cast<uint8_t *>(engineTrans->transBuff.get());
    bufferSize = engineTrans->bufferSize;
}
/**
* @ingroup hiaiengine
* @brief GetTransSearPtr, �����л�Trans����
* @param [in] : ctrl_ptr �ṹ��ָ��
* @param [in] : ctrlLen ���ݽṹ�п�����Ϣ��С
* @param [in] : data_ptr �ṹ������ָ��
* @param [in] : dataLen �ṹ��������Ϣ�洢�ռ��С��������У�飬����ʾԭʼ������Ϣ���?
* @param [out]��std::shared_ptr<void> ����Engine��ָ��ṹ��ָ��?
 */
inline std::shared_ptr<void> GetTransDearPtr(const char *ctrlPtr, const uint32_t &ctrlLen,
                                             const unsigned char *dataPtr, const uint32_t &dataLen)
{
    std::shared_ptr<EngineTransNewT> engineTransPtr = std::make_shared<EngineTransNewT>();
    engineTransPtr->bufferSize = ((EngineTransNewT *)(ctrlPtr))->bufferSize;
    engineTransPtr->transBuff.reset(const_cast<unsigned char *>(dataPtr), hiai::Graph::ReleaseDataBuffer);
    return std::static_pointer_cast<void>(engineTransPtr);
}

#endif