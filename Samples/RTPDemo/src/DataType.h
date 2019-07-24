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

#ifndef __ATLAS_ENCAP_AND_DECAP_DATA_TYPE_H__
#define __ATLAS_ENCAP_AND_DECAP_DATA_TYPE_H__

#include <cstddef>
#include <cstdint>

#define ATLAS_BIG_END 1

namespace Atlas {
namespace RTP {

#pragma pack(push, 1)
    struct RTPHeader{        // RTP Header Info
#ifdef ATLAS_BIG_END
        uint8_t CC: 4;
        uint8_t extension: 1;
        uint8_t padding: 1;
        uint8_t version: 2;
        uint8_t payload: 7;
        uint8_t mark: 1;
#else
        uint8_t version: 2;
        uint8_t padding: 1;
        uint8_t extension: 1;
        uint8_t CC: 4;
        uint8_t mark: 1;
        uint8_t payload: 7;
#endif
        uint16_t SN; // Sequence Nummer
        uint32_t timestamp;
        uint32_t SSRC;
        uint32_t CSRC;
    };

    struct H264NALU_Header {
#ifdef ATLAS_BIG_END
        uint8_t type : 5;
        uint8_t NRI : 2;
        uint8_t F : 1;
#else
        uint8_t F : 1;
        uint8_t NRI : 2;
        uint8_t type : 5;
#endif
    };

    struct H264_FU_Indicator {
#ifdef ATLAS_BIG_END
        uint8_t type : 5;
        uint8_t NRI : 2;
        uint8_t F : 1;
#else
        uint8_t F : 1;
        uint8_t NRI : 2;
        uint8_t type : 5;
#endif
    };

    struct H264_FUHeader{
#ifdef ATLAS_BIG_END
        uint8_t type : 5;
        uint8_t reserved : 1;
        uint8_t end : 1;
        uint8_t start : 1;
#else
        uint8_t start : 1;
        uint8_t end : 1;
        uint8_t reserved : 1;
        uint8_t type : 5;
#endif
    };

#pragma pack(pop)

    struct RTPPackage {
        RTPHeader summary;
        uint8_t* header = nullptr;
        size_t payloadOffset = 0;
        size_t payloadSize = 0;
    };

}
}

#endif