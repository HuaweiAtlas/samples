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

#include "DynamicGraph.h"
#include "hiaiengine/api.h"
#include "hiaiengine/ai_memory.h"
#include <proto/graph_config.pb.h>

using namespace std;
using namespace hiai;

static inline uint32_t GetDecodeEngineId(uint32_t decodeEngineIndex);
void AddHostEngine(GraphConfig* gConfig, uint32_t engineId, const string& engineName);
void AddDeviceEngine(GraphConfig* gConfig, uint32_t engineId, const string& engineName);
void AddDecodeEngine(GraphConfig* gConfig, TestType testType, uint32_t engineId);
void AddConnect(GraphConfig* gConfig, uint32_t srcId, uint32_t srcPort, uint32_t targetId, uint32_t targetPort);

DynamicGraph::DynamicGraph(uint32_t startChip,
    uint32_t endChip,
    uint32_t graphsPerChip,
    uint32_t threadsPerGraph,
    TestType testType)
{
    this->startChip = startChip;
    this->endChip = endChip;
    this->graphsPerChip = graphsPerChip;
    this->threadsPerGraph = threadsPerGraph;
    this->testType = testType;
    this->createTime.tv_sec = 0;
    this->createTime.tv_usec = 0;
}

HIAI_StatusT DynamicGraph::InitAndStartGraph()
{
    HIAI_StatusT status;
    shared_ptr<GraphConfigList> glist(new GraphConfigList());

    gettimeofday(&(this->createTime), NULL);

    for (uint32_t chipId = this->startChip; chipId <= this->endChip; chipId++) {
        status = HIAI_Init(chipId);
        if (status != HIAI_OK) {
            return status;
        }

        for (uint32_t graphIndex = 0; graphIndex < this->graphsPerChip; graphIndex++) {
            uint32_t graphId = this->GetGraphId(chipId, graphIndex);
            GraphConfig* gConfig = glist->add_graphs();
            gConfig->set_device_id(to_string(chipId));
            gConfig->set_graph_id(graphId);
            gConfig->set_priority(0);

            AddDeviceEngine(gConfig, g_srcEngineId, g_srcEngineName);
            AddHostEngine(gConfig, g_dstEngineId, g_dstEngineName);

            for (uint32_t dEngineIndex = 0; dEngineIndex < this->threadsPerGraph; dEngineIndex++) {
                uint32_t decodeEngineId = GetDecodeEngineId(dEngineIndex);
                AddDecodeEngine(gConfig, testType, decodeEngineId);
                AddConnect(gConfig, g_srcEngineId, dEngineIndex, decodeEngineId, 0);
                AddConnect(gConfig, decodeEngineId, 0, g_dstEngineId, 0);
            }
        }
    }

    status = Graph::CreateGraph(*glist);

    return status;
}

void DynamicGraph::SetAllGraphRecvFunctor(const shared_ptr<DataRecvInterface>& dataRecv)
{
    for (uint32_t chipId = startChip; chipId <= endChip; chipId++) {
        for (uint32_t graphIndex = 0; graphIndex < graphsPerChip; graphIndex++) {
            uint32_t graphId = GetGraphId(chipId, graphIndex);
            shared_ptr<Graph> graph = Graph::GetInstance(graphId);
            if (nullptr == graph) {
                cout << "Fail to get the graph" << endl;
                exit(1);
            }

            //
            hiai::EnginePortID targetPort;
            targetPort.graph_id = graphId;
            targetPort.engine_id = g_dstEngineId;
            targetPort.port_id = 0;
            int ret = graph->SetDataRecvFunctor(targetPort, dataRecv);
            if (HIAI_OK != ret) {
                cout << "Fail to SetDataRecvFunctor " << ret << endl;
                exit(1);
            }
        }
    }
}

void DynamicGraph::SendAllGraphData(unsigned char* buffer, int bufferSize, int w, int h, int f, int l)
{
    for (uint32_t chipId = startChip; chipId <= endChip; chipId++) {
        for (uint32_t graphIndex = 0; graphIndex < graphsPerChip; graphIndex++) {
            uint32_t graphId = GetGraphId(chipId, graphIndex);
            shared_ptr<Graph> graph = Graph::GetInstance(graphId);
            if (nullptr == graph) {
                cout << "Fail to get the graph" << endl;
                return;
            }
            //
            shared_ptr<GraphCtrlInfoT> ctrlInfo = make_shared<GraphCtrlInfoT>();
            ctrlInfo->chipId = chipId;
            ctrlInfo->graphId = graphId;
            ctrlInfo->dEngineCount = this->threadsPerGraph;
            int decodeCnt = 100;
            ctrlInfo->decodeCount = decodeCnt;
            ctrlInfo->bufferSize = bufferSize;
            ctrlInfo->dataBuff.reset(buffer, hiai::HIAIMemory::HIAI_DFree);

            ctrlInfo->width = w;
            ctrlInfo->height = h;
            ctrlInfo->format = f;
            ctrlInfo->level = l;

            EnginePortID enginePortId;
            enginePortId.graph_id = graphId;
            enginePortId.engine_id = g_srcEngineId;
            enginePortId.port_id = 0;
            int ret = graph->SendData(enginePortId, "GraphCtrlInfoT", static_pointer_cast<void>(ctrlInfo));
            if (HIAI_OK != ret) {
                cout << "Fail to send data" << ret << endl;
                return;
            }
        }
    }
}

void DynamicGraph::DestroyAllGraph()
{
    for (uint32_t chipId = startChip; chipId <= endChip; chipId++) {
        for (uint32_t graphIndex = 0; graphIndex < graphsPerChip; graphIndex++) {
            uint32_t graphId = this->GetGraphId(chipId, graphIndex);
            Graph::DestroyGraph(graphId);
        }
    }
}

uint32_t DynamicGraph::GetGraphId(uint32_t chipId, uint32_t graphIndex)
{
    int weightTime = 1000;
    int weightMod = 100000;
    int weightMul = 10000;
    int chipIdWeight = 100;
    return (uint32_t)(((createTime.tv_sec * weightTime + createTime.tv_usec / weightTime) % weightMod) * weightMul + (chipId) * chipIdWeight + graphIndex);
}

static inline uint32_t GetDecodeEngineId(uint32_t decodeEngineIndex)
{
    return g_decodeEngineId + decodeEngineIndex;
}

void AddHostEngine(GraphConfig* gConfig, uint32_t engineId, const string& engineName)
{
    EngineConfig* engine = gConfig->add_engines();
    engine->set_id(engineId);
    engine->set_engine_name(engineName);
    engine->set_side(EngineConfig_RunSide_HOST);
    engine->set_thread_num(1);
}

void AddDeviceEngine(GraphConfig* gConfig, uint32_t engineId, const string& engineName)
{
    EngineConfig* engine = gConfig->add_engines();
    engine->set_id(engineId);
    engine->set_engine_name(engineName);
    engine->set_side(EngineConfig_RunSide_DEVICE);
    engine->set_thread_num(1);
    engine->add_so_name(g_deviceLibName);
}

void AddDecodeEngine(GraphConfig* gConfig, TestType testType, uint32_t engineId)
{
    if (testType == DECODE_PIC_JPEG) {
        AddDeviceEngine(gConfig, engineId, g_jpegdEngineName);
    } else if (testType == DECODE_PIC_PNG) {
        AddDeviceEngine(gConfig, engineId, g_pngdEngineName);
    } else if (testType == DECODE_VIDEO_H264) {
        AddDeviceEngine(gConfig, engineId, g_vdecEngineName);
    } else if (testType == DECODE_VIDEO_H265) {
        AddDeviceEngine(gConfig, engineId, g_vdecEngineName);
    } else if(testType == ENCODE_PIC_JPEG) {
        AddDeviceEngine(gConfig, engineId, g_jpegeEngineName);
    }
}

void AddConnect(GraphConfig* gConfig, uint32_t srcId, uint32_t srcPort, uint32_t targetId, uint32_t targetPort)
{
    ConnectConfig* connect = gConfig->add_connects();
    connect->set_src_engine_id(srcId);
    connect->set_src_port_id(srcPort);
    connect->set_target_engine_id(targetId);
    connect->set_target_port_id(targetPort);
}
