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

#ifndef DYNAMIC_GRAPH_H
#define DYNAMIC_GRAPH_H

#include "AppCommon.h"
#include <sys/time.h>
#include "hiaiengine/graph.h"

static string g_deviceLibName = "./libDevice.so";
static uint32_t g_srcEngineId = 100;
static string g_srcEngineName = "SrcEngine";
static uint32_t g_decodeEngineId = 500;
static string g_jpegdEngineName = "JpegdEngine";
static string g_pngdEngineName = "PngdEngine";
static string g_vdecEngineName = "VdecEngine";
static string g_jpegeEngineName = "JpegeEngine";
static uint32_t g_dstEngineId = 900;
static string g_dstEngineName = "DstEngine";

class DynamicGraph {
public:
    DynamicGraph(uint32_t startChip, uint32_t endChip, uint32_t graphsPerChip, uint32_t threadsPerGraph, TestType testType);
    HIAI_StatusT InitAndStartGraph();
    void SetAllGraphRecvFunctor(const std::shared_ptr<DataRecvInterface>& dataRecv);
    void SendAllGraphData(unsigned char* buffer, int bufferSize, int w, int h, int f, int l);
    void DestroyAllGraph();
    uint32_t GetGraphId(uint32_t chipId, uint32_t graphIndex);
    ~DynamicGraph(){}
private:
    uint32_t startChip;
    uint32_t endChip;
    uint32_t graphsPerChip;
    uint32_t threadsPerGraph;
    TestType testType;
    struct timeval createTime;
};

#endif
