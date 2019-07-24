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

#include <iostream>
#include <cstring>
#include<netinet/in.h>

#include "InnerRtp.h"
#include "RtpPayloadParser.h"

namespace Atlas {
namespace RTP {

    InnerUnpacking::InnerUnpacking(const std::map<uint8_t, std::string>& payload)
    {
        UnpackingInfo tmpUnpackInfo;
        for (auto& item : payload) {
            tmpUnpackInfo.name = item.second;

            CALLBACK_PARSE func = GetParser(tmpUnpackInfo.name);
            if (func != nullptr) {
                tmpUnpackInfo.func = func;
            }

            unpackInfo.emplace(item.first, tmpUnpackInfo);
        }
    }

    InnerUnpacking::~InnerUnpacking() {
        unpackInfo.clear();
    }

    bool InnerUnpacking::SetFrameBuffer(uint8_t payload, uint8_t* buffer, const size_t len) {
        auto itr = unpackInfo.find(payload);
        if (itr != unpackInfo.end()) {
            itr->second.frameInfo.data = buffer;
            itr->second.frameInfo.size = len;
            itr->second.frameInfo.offset = 0;
            return true;
        }
        else {
            std::cout << "unsupport payload( " << unsigned(payload) << ")" << std::endl;
        }
        return false;
    }

    // 外部保证输入的有效性
    // in != nullptr && inLen >=4
    bool InnerUnpacking::CheckPkg(uint8_t* in, size_t& rtpLen) {
        if ((in[0] == '$') && (in[1] == 0)) { // 0x00: Video RTP; 0x01: Video RTCP; 0x02 Audio RTP; 0x03: Audio RTCP ,现在仅仅支持 Video RTP
            rtpLen = ((in[2] & 0xFF) << 8) + (in[3] & 0xFF);
            return (rtpLen < MAX_MTU);
        }
        else {
            return false;
        }
    }

    bool InnerUnpacking::InsertPkg(uint8_t* buffer, const size_t len) {

        RTPPackage tmpPkg;
        if (!GetRtpHeader(tmpPkg, buffer, len)) {
            std::cout << "GetRtphead failed." << std::endl;
            return false;
        }

        // check if the rtp package is effective
        if (!CheckRTPHeader(tmpPkg.summary)) {
            std::cout << "check rtp header failed." << std::endl;
            return false;
        }

        if (!HandleOnePackage(tmpPkg)) {
            std::cout << "handle one package failed." << std::endl;
            SetFrameInvalid(tmpPkg.summary.payload);
        }
        return true;
    }

    bool InnerUnpacking::HandleOnePackage(RTPPackage& pkg) {
        // 
        bool isStart = false;
        bool isKeyFrame = false;

        size_t len = 0;
        uint8_t* payloadHeader = nullptr;
        uint8_t payload = pkg.summary.payload;
        auto itr = unpackInfo.find(payload);
        if (itr != unpackInfo.end()) {
            len = itr->second.func(pkg, payloadHeader, isStart, isKeyFrame);
            if ((len <= 0) || (payloadHeader == nullptr)) {
                std::cout << "decode failed. (curLen = " << len << ", size = " << pkg.payloadSize << ")" << std::endl;
                return false;
            }
        }

        if (isStart) {
            WriteIfFrameOK(payload);
        }

        if (false == WriteBuffer(payload, isKeyFrame, payloadHeader, len)) {
            return false;
        }

        return true;
    }

    void InnerUnpacking::SetFrameInvalid(uint8_t payload) {
        auto itr = unpackInfo.find(payload);
        if (itr != unpackInfo.end()) {
            itr->second.frameInfo.isValid = false;
        }
    }

    void InnerUnpacking::WriteIfFrameOK(const uint8_t payload) {
        auto itr = unpackInfo.find(payload);
        if (itr != unpackInfo.end()) {
            if (itr->second.frameInfo.isValid){
                if (itr->second.frameInfo.offset > 0) {
                    // 现在audioPayload上用的是H265,对于H265,尾部需要补0,可以考虑后续加到各自类型的函数中
                    if ((itr->second.frameInfo.data[itr->second.frameInfo.offset-1] != 0) && (itr->second.frameInfo.offset < itr->second.frameInfo.size)){
                        if (0 == strcmp(itr->second.name.c_str(), "h265")) {
                            itr->second.frameInfo.data[itr->second.frameInfo.offset] = 0;
                            itr->second.frameInfo.offset += 1;
                        }
                    }
                    callbackWrite(payload, itr->second.frameInfo.isKeyFrame, itr->second.frameInfo.data, itr->second.frameInfo.offset);
                }
            }
            else {
                if (itr->second.frameInfo.offset != 0) { // when offset == 0 && isValid == false, it's the first frame.
                    std::cout << "get a invalid frame.(len= " << itr->second.frameInfo.offset <<")" << std::endl;
                }
                itr->second.frameInfo.isValid = true;
            }
            itr->second.frameInfo.offset = 0;
            itr->second.frameInfo.isKeyFrame = false;
        }
    }

    bool InnerUnpacking::WriteBuffer(const uint8_t payload, const bool isKeyFrame, const uint8_t* buffer, const size_t size) {
        auto itr = unpackInfo.find(payload);
        if (itr != unpackInfo.end()) {
            if ((itr->second.frameInfo.offset + size) > itr->second.frameInfo.size) {
                std::cout << "frame buffer is too small. (" <<  itr->second.frameInfo.offset << " + " << size << " > "<< itr->second.frameInfo.size << "), so drop this frame and reset offset." << std::endl;
                itr->second.frameInfo.offset = 0;
                return false;
            }

            size_t remainLen = itr->second.frameInfo.size - itr->second.frameInfo.offset;
            size_t copyLen = (remainLen > size) ? size : remainLen;
            if (copyLen != size)
            {
                std::cout << "memcpy failed, need resize buffer. (offset = " << itr->second.frameInfo.offset << ", size = " << itr->second.frameInfo.size << ", bufSize = " << size << ")" << std::endl;
            }
            memcpy(itr->second.frameInfo.data + itr->second.frameInfo.offset, buffer, copyLen);
            if (isKeyFrame) {
                itr->second.frameInfo.isKeyFrame = isKeyFrame;
            }
            itr->second.frameInfo.offset += copyLen;
            return true;
        }
        std::cout << "find buffer failed. (payload=" << unsigned(payload) << ")" << std::endl;
        return false;
    }


    bool InnerUnpacking::GetRtpHeader(RTPPackage& pkg, uint8_t* buffer, size_t len) {

        if (buffer[0] == '$') {
            buffer += INTERLEAVED_LEN;
            len -= INTERLEAVED_LEN;
        }

        if (len < (sizeof(RTPHeader) - sizeof(uint32_t))) {
            return false;
        }

        memcpy(&pkg.summary, buffer, sizeof(RTPHeader) - sizeof(uint32_t));

        pkg.summary.SN = ntohs(pkg.summary.SN);
        pkg.summary.timestamp = ntohl(pkg.summary.timestamp);
        pkg.summary.SSRC = ntohl(pkg.summary.SSRC);
        pkg.payloadOffset = (pkg.summary.extension)? 4 * sizeof(uint32_t) : 3 * sizeof(uint32_t);
        pkg.header = buffer;
        if (pkg.summary.extension) {
            std::cout << "now is not support extension." << std::endl;
            pkg.summary.CSRC = ntohl(pkg.summary.CSRC);
            return false;
        }
        pkg.payloadSize = len - pkg.payloadOffset;
        return true;
    }

    bool InnerUnpacking::CheckRTPHeader(const RTPHeader header) {

        uint32_t SSRC = header.SSRC;
        auto itr = unpackInfo.find(header.payload);
        if (itr != unpackInfo.end()) {
            if (itr->second.SSRC == SSRC) {
                return true;
            }
            else {
                if (!itr->second.isSSRCInited) {
                    itr->second.SSRC = SSRC;
                    itr->second.isSSRCInited = true;
                    return true;
                }

                decltype(illegalSSRC)::iterator illegalItr = illegalSSRC.find(SSRC);
                if (illegalItr == illegalSSRC.end()) {
                    illegalSSRC.insert(SSRC);
                    std::cout << "new SSRC is not match. (" << SSRC << " <-> " << itr->second.SSRC << ")" << std::endl;
                }
                return false;
            }
        }
        std::cout << "check RTP header failed.(payload is illage.[" << unsigned(header.payload) << "])" << std::endl;
        return false;
    }

    InnerUnpacking::CALLBACK_PARSE InnerUnpacking::GetParser(const std::string payloadName) {
        if (0 == strcmp(payloadName.c_str(), "h264")) {
            return H264Parser::SetPayloadData;
        }
        else if (0 == strcmp(payloadName.c_str(), "h265")) {
            return H265Parser::SetPayloadData;
        }
        else {
            return nullptr;
        }
    }

}
}