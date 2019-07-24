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

#include <arpa/inet.h>
#include <iostream>
#include <cstring>

#include "RtpPayloadParser.h"

namespace Atlas {
namespace RTP {

    namespace {
        constexpr size_t NAL_HEAD_SIZE = 2;
        constexpr size_t H265_FU_IND_SIZE = 3;
        constexpr size_t H265_PAYLOAD_HEADER_SIZE = 2;
        constexpr size_t H265_FU_HEADER_SIZE = 1;

        static const uint8_t h264StartSeq[] = { 0, 0, 0, 1 };
        static const uint8_t h265StartSeq[] = { 0, 0, 1 };
    }

    size_t H264Parser::SetPayloadData(RTPPackage& pkg, uint8_t*&buffer, bool& isStart, bool& isKeyFrame) {
        isStart = false;
        isKeyFrame = false;

        uint8_t* srcData = pkg.header + pkg.payloadOffset;
        size_t srcSize = pkg.payloadSize;

        // remove start code
        size_t offset = CommonParser::CheckStartCode(srcData);
        srcData += offset;
        srcSize -= offset;

        if (srcSize < 1) {
            std::cout << "srcSize = " << srcSize << std::endl;
            return 0;
        }

        //
        size_t retLen = 0;
        uint8_t nal = srcData[0];
        uint8_t type = nal & 0x1f;
        if ((type == 5) || (type == 7)) {
            isKeyFrame = true;
        }
        if (type >= 1 && type <=23) { // H264 nal
            type = 1;
        }
        switch (type) {
        case 1:
            {
                buffer = srcData - sizeof(h264StartSeq);
                ((uint32_t*)buffer)[0] = ntohl(0x1);
                retLen = srcSize + sizeof(h264StartSeq);
                isStart = true;
            }
            break;
        case 24: // STAP-A (one packet, multiple nals)
            std::cout << "STAP-A is not support now." << std::endl;
            break;
        case 28: // FU-A (fragmented nal)
            {
                if (srcSize <= NAL_HEAD_SIZE) {
                    std::cout << "srcSize = " << srcSize << std::endl;
                }

                ++srcData;
                --srcSize;
                H264_FUHeader* fuHeader = (H264_FUHeader*)srcData;
                if (fuHeader->start) {
                    ++srcData;
                    --srcSize;
                    uint8_t reconstructNal = (nal & 0xe0) | fuHeader->type;
                    // startCode + reconstructNal + buf(payload+2)
                    buffer = srcData - sizeof(h264StartSeq) - 1; // TODO:
                    buffer[sizeof(h264StartSeq)] = reconstructNal;
                    if (reconstructNal == 0x67) {
                        isKeyFrame = true;
                    }
                    ((uint32_t*)buffer)[0] = ntohl(0x1);
                    isStart = true;
                }
                else {
                    buffer = srcData + 1;
                }
                retLen = pkg.header + pkg.payloadOffset + pkg.payloadSize - buffer;
            }
            break;

        default:
            std::cout << "H264 nal type(" << type << ") is not support now." << std::endl;
            break;
        }
        return retLen;
    }

    size_t H265Parser::SetPayloadData(RTPPackage& pkg, uint8_t*&buffer, bool& isStart, bool& isKeyFrame) {
        isStart = false;
        isKeyFrame = false;

        uint8_t* srcData = pkg.header + pkg.payloadOffset;
        size_t srcSize = pkg.payloadSize;

        // sanity check for size of input packet: 1 byte payload at least
        if (srcSize <= H265_PAYLOAD_HEADER_SIZE + 1)
        {
            std::cout << "srcSize = " << srcSize << std::endl;
            return 0;
        }

        // remove start code
        size_t offset = CommonParser::CheckStartCode(srcData);
        srcData += offset;
        srcSize -= offset;

        const uint8_t *rtp_pl = srcData;
        int nal_type = (srcData[0] >> 1) & 0x3f;
        int lid = ((srcData[0] << 5) & 0x20) | ((srcData[1] >> 3) & 0x1f);
        int tid = srcData[1] & 0x07;

        /* sanity check for correct layer ID */
        if (lid)
        {
            /* future scalable or 3D video coding extensions */
            std::cout << "illegal lid" << std::endl;
            return 0;
        }

        /* sanity check for correct temporal ID */
        if (!tid)
        {
            std::cout << "illegal tid" << std::endl;
            return 0;
        }

        /* sanity check for correct NAL unit type */
        if (nal_type > 50)
        {
            std::cout << "illegal nal type." << std::endl;
            return 0;
        }

        size_t retLen = 0;
        uint8_t first_fragment = 0;
        uint8_t last_fragment = 0;
        uint8_t fu_type = 0;
        if ((16 <= nal_type) && (nal_type <= 21)) {
            isKeyFrame = true;
        }
        if (33 == nal_type) {
            isKeyFrame = true;
        }
        switch (nal_type)
        {
        /* video parameter set (VPS) */
        case 32:
        /* sequence parameter set (SPS) */
        case 33:
        /* picture parameter set (PPS) */
        case 34:
        /*  supplemental enhancement information (SEI) */
        case 39:
        /* single NAL unit packet */
        default:
            buffer = srcData - sizeof(h265StartSeq);
            memcpy(buffer, h265StartSeq, sizeof(h265StartSeq));
            retLen = srcSize + sizeof(h265StartSeq);
            isStart = true;
            break;
        /* aggregated packet (AP) - with two or more NAL units */
        case 48:
            std::cout << "STAP-A is not support now." << std::endl;
            break;
        /* fragmentation unit (FU) */
        case 49:
            /* pass the HEVC payload header */
            srcData += H265_PAYLOAD_HEADER_SIZE;
            srcSize -= H265_PAYLOAD_HEADER_SIZE;

            /*
         *    decode the FU header
         *
         *     0 1 2 3 4 5 6 7
         *    +-+-+-+-+-+-+-+-+
         *    |S|E|  FuType   |
         *    +---------------+
         *
         *       Start fragment (S): 1 bit
         *       End fragment (E): 1 bit
         *       FuType: 6 bits
         */
            first_fragment = srcData[0] & 0x80;
            last_fragment = srcData[0] & 0x40;
            fu_type = srcData[0] & 0x3f;

            /* pass the HEVC FU header */
            srcData += H265_FU_HEADER_SIZE;
            srcSize -= H265_FU_HEADER_SIZE;

            /* pass the HEVC DONL field */

            /* sanity check for size of input packet: 1 byte payload at least */
            if (srcSize <= 0)
            {
                std::cout << "srcSize= " << srcSize << std::endl;
                return 0;
            }

            if (first_fragment && last_fragment)
            {
                std::cout << "fragment err." << std::endl;
                return 0;
            }

            uint8_t new_nal_header[2];
            new_nal_header[0] = (rtp_pl[0] & 0x81) | (fu_type << 1);
            new_nal_header[1] = rtp_pl[1];

            if(first_fragment)
            {
                buffer = srcData - sizeof(h265StartSeq) - H265_PAYLOAD_HEADER_SIZE;
                buffer[sizeof(h265StartSeq)] = new_nal_header[0];
                buffer[sizeof(h265StartSeq) + 1] = new_nal_header[1];
                memcpy(buffer, h265StartSeq, sizeof(h265StartSeq));
                isStart = true;

                uint8_t nalType = (new_nal_header[0] & 0x7E) >> 1;
                if ((nalType == 0x21) || ((16 <= nalType) && (nalType <= 21))) {
                    isKeyFrame = true;
                }
            }
            else
            {
                buffer = srcData;
            }
            retLen = pkg.header + pkg.payloadOffset + pkg.payloadSize - buffer;
            break;
        /* PACI packet */
        case 50:
            /* Temporal scalability control information (TSCI) */
            std::cout << "not support now." << std::endl;
            break;
        }

        return retLen;
    }

    size_t CommonParser::CheckStartCode(uint8_t* data) {
        uint32_t* pStartCode = (uint32_t*)data;
        uint32_t startCode = ntohl(pStartCode[0]);
        if (startCode == 0x01) {
            return sizeof(uint32_t);
        }
        else if ((startCode & ~0xFF) == 0x100) {
            return 3;
        }
        else {
            return 0;
        }

    }
}
}