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

#ifndef __ATLAS_INNER_RTP_H__
#define __ATLAS_INNER_RTP_H__

#include <map>
#include <set>
#include <functional>

#include "Rtp.h"
#include "DataType.h"


namespace Atlas{
namespace RTP {

    constexpr size_t INTERLEAVED_LEN = 4;
    constexpr size_t MAX_MTU = 1500;
    
    struct FrameInfo {
        uint8_t* data = nullptr;
        size_t size = 0;
        size_t offset = 0;
        bool isKeyFrame = false;
        bool isValid = false;
    };

    enum class PayloadType
    {
        H264,
        H265,
        NONE
    };

    class InnerUnpacking : public Unpacking {
    public:
        using CALLBACK_PARSE = std::function<size_t(RTPPackage&, uint8_t*&, bool&, bool&)>;
        struct UnpackingInfo
        {
            std::string name;
            FrameInfo frameInfo;
            CALLBACK_PARSE func;
            uint32_t SSRC = 0;
            uint16_t seq = 0;
            bool isSSRCInited = false;
        };

        // payload != 0
        explicit InnerUnpacking(const std::map<uint8_t, std::string>& payload);
        ~InnerUnpacking();

        bool SetFrameBuffer(uint8_t payload, uint8_t* buffer, const size_t len) override;
        // 需要将切割好的包放入,用CheckPkg检测是否符合要求
        bool CheckPkg(uint8_t* in, size_t& rtpLen) override;
        bool InsertPkg(uint8_t* buffer, const size_t len) override;
        void SetCallbackWrite(const CALLBACK_WRITE& cb) override {callbackWrite = cb;}

    private:
        void WriteIfFrameOK(const uint8_t payload);
        bool WriteBuffer(const uint8_t payload, const bool isKeyFrame, const uint8_t* buffer, const size_t size);
        bool HandleOnePackage(RTPPackage& pkg);
        bool GetRtpHeader(RTPPackage& pkg, uint8_t* buffer, size_t len);
        bool CheckRTPHeader(const RTPHeader header);
        CALLBACK_PARSE GetParser(const std::string payloadName);
        void SetFrameInvalid(uint8_t payload);


    private:
        std::map<uint8_t, UnpackingInfo> unpackInfo;

        std::set<uint32_t> illegalSSRC;
        CALLBACK_WRITE callbackWrite;
    };

}
}


#endif