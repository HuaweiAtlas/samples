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

#include <sys/time.h>
#include "hiaiengine/graph.h"
#include "hiaiengine/api.h"
#include <proto/graph_config.pb.h>

static const std::string DEVICE_LIB_NAME = "./libDevice.so";
static const uint32_t SRC_ENGINE_ID = 100;
static const std::string SRC_ENGINE_NAME = "SrcEngine";
static const uint32_t DECODE_ENGINE_ID = 500;
static const std::string VDEC_ENGINE_NAME = "VDecEngine";
static const std::string VENC_ENGINE_NAME = "VencEngine";
static const uint32_t DST_ENGINE_ID = 900;
static const std::string DST_ENGINE_NAME = "DstEngine";

class DynamicGraph {
public:
    DynamicGraph(uint32_t startChip, uint32_t endChip, uint32_t graphsPerChip, 
        uint32_t threadsPerGraph, uint32_t testType, uint32_t deOrEnc, uint32_t height, uint32_t width);
    
    ~DynamicGraph();

    HIAI_StatusT InitAndStartGraph();

    HIAI_StatusT SetAllGraphRecvFunctor(const std::shared_ptr<hiai::DataRecvInterface>& dataRecv);

    HIAI_StatusT SendAllGraphData(unsigned char* buffer, int bufferSize);

    void DestroyAllGraph();

    uint32_t GetGraphId(uint32_t chipId, uint32_t graphIndex);
private:

    void AddHostEngine(hiai::GraphConfig* gConfig, uint32_t engineId, const std::string& engineName);

    void AddDeviceEngine(hiai::GraphConfig* gConfig, uint32_t engineId, const std::string& engineName);

    void AddDecodeEngine(hiai::GraphConfig* gConfig, uint32_t testType, uint32_t engineId);

    void AddEncodeEngine(hiai::GraphConfig* gConfig, uint32_t engineId);

    void AddConnect(hiai::GraphConfig* gConfig, uint32_t srcId, uint32_t srcPort, 
        uint32_t targetId, uint32_t targetPort);

    uint32_t GetDecodeEngineId(uint32_t decodeEngineIndex);

    uint32_t startChip;
    uint32_t endChip;
    uint32_t graphsPerChip;
    uint32_t threadsPerGraph;
    uint32_t testType;
    uint32_t deOrEnc;
    uint32_t height;
    uint32_t width;
    struct timeval* createTime;
};

#endif
