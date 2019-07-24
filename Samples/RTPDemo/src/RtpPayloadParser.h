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

#ifndef __ATLAS_RTP_PAYLOAD_PARSER_H__
#define __ATLAS_RTP_PAYLOAD_PARSER_H__

/* 对于H264支持单帧和FU_A两种形式
 * 对于H265支持单帧和FU两种形式
 * 与智能小站对接时，发送端仅发送这两种格式的RTP包
*/


#include "DataType.h"

namespace Atlas {
namespace RTP {

    /* 对于RTP而言，收到的码流是不带起始码的
     * 但是在整帧存储时，需要在其头部添加起始码
    */
    class H264Parser {
    public:
        static size_t SetPayloadData(RTPPackage& pkg, uint8_t*&outBuffer, bool& isStart, bool& isEnd);
    private:
        H264Parser() = delete;
        ~H264Parser() = delete;
    };

    class H265Parser {
    public:
        static size_t SetPayloadData(RTPPackage& pkg, uint8_t*&outBuffer, bool& isStart, bool& isEnd);
    private:
        H265Parser() = delete;
        ~H265Parser() = delete;
    };

    class CommonParser {
    public:
        // return offset of startcode
        static size_t CheckStartCode(uint8_t* buffer);
    private:
        CommonParser() = delete;
        ~CommonParser() = delete;
    };

}
}

#endif