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

#ifndef COMMON_DATA_TYPE_H_x
#define COMMON_DATA_TYPE_H_x

#include "RawDataBufferHigh.h"
#include "stream_data.h"
#include "dvpp/dvpp_config.h"
#include <vector>
#include <memory>


#define YUV_BYTES 1.5
#define BGR_BYTES 3
#define RGB_BYTES 3

#define H264 0
#define H265 1

template <class T>
inline void bboxToSquare(T& xmin, T& ymin, T& xmax, T& ymax)
{
    T w = xmax - xmin;
    T h = ymax - ymin;
    T maxSide = std::max(w, h);
    xmin = xmin + (w - maxSide) * 0.5;
    ymin = ymin + (h - maxSide) * 0.5;
    xmax = xmin + maxSide;
    ymax = ymin + maxSide;
}

template <class T>
inline void bboxToSquare(Rectangle<T>& rect)
{
    bboxToSquare(rect.anchor_lt.x, rect.anchor_lt.y,
        rect.anchor_rb.x, rect.anchor_rb.y);
}

template <class T>
inline void checkBound(Rectangle<T>& rect, int32_t upBoundW, int32_t upBoundH, int32_t lowBoundW = 0, int32_t lowBoundH = 0)
{
    rect.anchor_lt.x = std::max(rect.anchor_lt.x, lowBoundW);
    rect.anchor_lt.y = std::max(rect.anchor_lt.y, lowBoundH);
    rect.anchor_rb.x = std::min(rect.anchor_rb.x, upBoundW);
    rect.anchor_rb.y = std::min(rect.anchor_rb.y, upBoundH);
}

struct FaceTrack {
    uint64_t id;
    uint32_t age;
    uint32_t totalVisibleCount;
    uint32_t consecutiveInvisibleCount;
    shared_ptr<FaceObject> face;

    FaceTrack(uint64_t id_, const shared_ptr<FaceObject>& face_)
        : id(id_)
        , age(1)
        , totalVisibleCount(1)
        , consecutiveInvisibleCount(0)
        , face(face_)
    {
    }
};


#endif
