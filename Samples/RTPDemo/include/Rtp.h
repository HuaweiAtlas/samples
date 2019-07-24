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

#ifndef __ATLAS_RTP_H__
#define __ATLAS_RTP_H__

/* 该类用于接收有TCP传输过来的RTP的数据流
 * 不同于通过UDP传输的RTP包(TCP有粘包的现象)，
 * RTP over TCP 需要定义 Interleaved，
 * 参考wiki的定义 https://en.wikipedia.org/wiki/Real_Time_Streaming_Protocol
 * Stream data such as RTP packets is encapsulated by 
 * an ASCII dollar sign (24 hexadecimal), 
 * followed by a one-byte channel identifier, 
 * followed by the length of the encapsulated binary data as a binary two-byte integer in network byte order.
 * 即在RTP包头部再添加4个Bit
 *
 * 对于RTP包的格式，可以参考 RFC1889
 *  

 * 对于该类的使用：
 * 前置条件：
 * 1. 调用SetFrame分配对应的Frame空间(该空间由外部申请，考虑后续)
 * 2. 设置回调函数
 * 接收步骤：
 * 1. 当通过TCP收到一个包时，将其缓存
 * 2. 当buffer>MAX_MTU可以调用类中CheckPkg获取长度
 * 3. 当CheckPkg返回ok，则将足够的长度输入InsertPkg中
 * 4. InsertPkg会判断这个RTP包是否合理，如果合理则放入上述SetFrame准备的空间中
 * 5. 当完成一帧数据的拼接时，并且拼接过程没有异常发生，调用设置的回调函数
 * PS: 
 * 1. 暂不支持关键帧的识别(但是预留了接口)
 * 2. 通过TCP传输，不进行RTP头中的seq的检测
 */

#include <map>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace Atlas {
namespace RTP {
    class Unpacking {
    public:
        using CALLBACK_WRITE = std::function<void(uint8_t/*payload*/, bool /*isKeyFrame*/, uint8_t* /*buffer*/,size_t /*length*/)>;

        Unpacking() = default;
        virtual ~Unpacking() = default;

        virtual bool SetFrameBuffer(uint8_t payload, uint8_t* buffer, const size_t len) = 0;
        virtual bool CheckPkg(uint8_t* in, size_t& rtpLen) = 0;
        virtual bool InsertPkg(uint8_t* buffer, const size_t len) = 0;
        virtual void SetCallbackWrite(const CALLBACK_WRITE& cb) = 0;
    };


    class RtpFactory {
    public:
        static Unpacking* CreateUnpacking(const std::map<uint8_t, std::string>& payload);
    };


}
}


#endif