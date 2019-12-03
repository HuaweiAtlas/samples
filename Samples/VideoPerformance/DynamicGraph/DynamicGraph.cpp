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
#include "AppCommon.h"

using namespace std;
using namespace hiai;
HIAI_REGISTER_DATA_TYPE("GraphCtrlInfoT", GraphCtrlInfo);

static const int DECODEPERENGINE = 300;
static const int SEC2MS = 1000;
static const int HUNDRED = 100;
static const int HUNDRED_THOUSAND = 100000;
static const int TEN_THOUSAND = 10000;

DynamicGraph::DynamicGraph(uint32_t startChip, uint32_t endChip, uint32_t graphsPerChip, uint32_t threadsPerGraph,
    uint32_t testType, uint32_t deOrEnc, uint32_t height, uint32_t width)
    : startChip(startChip), endChip(endChip), graphsPerChip(graphsPerChip), threadsPerGraph(threadsPerGraph), 
      testType(testType), deOrEnc(deOrEnc), height(height), width(width), createTime(nullptr)
{

}

DynamicGraph::~DynamicGraph()
{
    if (createTime != nullptr) {
        delete createTime;
    }
}

HIAI_StatusT DynamicGraph::InitAndStartGraph()
{
    HIAI_StatusT status;
    if (createTime == nullptr) {
        createTime = new struct timeval;
    }
    int ret = gettimeofday((this->createTime), NULL);
    shared_ptr<GraphConfigList> glist(new GraphConfigList());

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

            AddHostEngine(gConfig, DST_ENGINE_ID, DST_ENGINE_NAME);
            AddDeviceEngine(gConfig, SRC_ENGINE_ID, SRC_ENGINE_NAME);

            for (uint32_t dEngineIndex = 0; dEngineIndex < this->threadsPerGraph; dEngineIndex++) {
                uint32_t decodeEngineId = GetDecodeEngineId(dEngineIndex);
                if (deOrEnc) {
                    AddEncodeEngine(gConfig, decodeEngineId);
                } else {
                    AddDecodeEngine(gConfig, testType, decodeEngineId);
                }
                AddConnect(gConfig, SRC_ENGINE_ID, dEngineIndex, decodeEngineId, 0);
                AddConnect(gConfig, decodeEngineId, 0, DST_ENGINE_ID, 0);
            }
        }
    }

    status = Graph::CreateGraph(*glist);

    return status;
}

HIAI_StatusT DynamicGraph::SetAllGraphRecvFunctor(const shared_ptr<DataRecvInterface>& dataRecv)
{
    for (uint32_t chipId = startChip; chipId <= endChip; chipId++) {
        for (uint32_t graphIndex = 0; graphIndex < graphsPerChip; graphIndex++) {
            uint32_t graphId = GetGraphId(chipId, graphIndex);
            shared_ptr<Graph> graph = Graph::GetInstance(graphId);
            if (nullptr == graph) {
                printf("Fail to get the graph\n");
                return HIAI_ERROR;
            }

            //
            hiai::EnginePortID targetPort;
            targetPort.graph_id = graphId;
            targetPort.engine_id = DST_ENGINE_ID;
            targetPort.port_id = 0;
            int ret = graph->SetDataRecvFunctor(targetPort, dataRecv);
            if (HIAI_OK != ret) {
                printf("Fail to SetDataRecvFunctor %d\n", ret);
                return HIAI_ERROR;
            }
        }
    }
    return HIAI_OK;
}

HIAI_StatusT DynamicGraph::SendAllGraphData(unsigned char* buffer, int bufferSize)
{
    for (uint32_t chipId = startChip; chipId <= endChip; chipId++) {
        for (uint32_t graphIndex = 0; graphIndex < graphsPerChip; graphIndex++) {
            uint32_t graphId = GetGraphId(chipId, graphIndex);
            shared_ptr<Graph> graph = Graph::GetInstance(graphId);
            if (nullptr == graph) {
                printf("Fail to get the graph\n");
                return HIAI_ERROR;
            }

            shared_ptr<GraphCtrlInfoT> ctrlInfo = make_shared<GraphCtrlInfoT>();
            ctrlInfo->chipId = chipId;
            ctrlInfo->graphId = graphId;
            ctrlInfo->dEngineCount = this->threadsPerGraph;
            ctrlInfo->decodeCount = DECODEPERENGINE * ctrlInfo->dEngineCount;
            ctrlInfo->height = height;
            ctrlInfo->width = width;
            ctrlInfo->bufferSize = bufferSize;
            ctrlInfo->dataBuff.reset(buffer, hiai::HIAIMemory::HIAI_DFree);
            EnginePortID enginePortId;
            enginePortId.graph_id = graphId;
            enginePortId.engine_id = SRC_ENGINE_ID;
            enginePortId.port_id = 0;
            int ret = graph->SendData(enginePortId, "GraphCtrlInfoT", static_pointer_cast<void>(ctrlInfo));
            if (HIAI_OK != ret) {
                printf("Fail to send data\n");
                return HIAI_ERROR;
            }
        }
    }
    return HIAI_OK;
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
    return (((uint32_t)(createTime->tv_sec) * SEC2MS + (uint32_t)(createTime->tv_usec) / SEC2MS) % HUNDRED_THOUSAND) 
        * TEN_THOUSAND + (chipId) * HUNDRED + graphIndex;
}

uint32_t DynamicGraph::GetDecodeEngineId(uint32_t decodeEngineIndex)
{
    return DECODE_ENGINE_ID + decodeEngineIndex;
}

void DynamicGraph::AddHostEngine(GraphConfig* gConfig, uint32_t engineId, const string& engineName)
{
    EngineConfig* engine = gConfig->add_engines();
    engine->set_id(engineId);
    engine->set_engine_name(engineName);
    engine->set_side(EngineConfig_RunSide_HOST);
    engine->set_thread_num(1);
    std::string soName = "lib" + engineName + ".so";
}

void DynamicGraph::AddDeviceEngine(GraphConfig* gConfig, uint32_t engineId, const string& engineName)
{
    EngineConfig* engine = gConfig->add_engines();
    engine->set_id(engineId);
    engine->set_engine_name(engineName);
    engine->set_side(EngineConfig_RunSide_DEVICE);
    engine->set_thread_num(1);
    engine->add_so_name(DEVICE_LIB_NAME);
}

void DynamicGraph::AddDecodeEngine(GraphConfig* gConfig, uint32_t testType, uint32_t engineId)
{
    if (testType == 0) {
        AddDeviceEngine(gConfig, engineId, VDEC_ENGINE_NAME);
    } else if (testType == 1) {
        AddDeviceEngine(gConfig, engineId, VDEC_ENGINE_NAME);
    }
}

void DynamicGraph::AddEncodeEngine(GraphConfig* gConfig, uint32_t engineId)
{
    AddDeviceEngine(gConfig, engineId, VENC_ENGINE_NAME);
}

void DynamicGraph::AddConnect(GraphConfig* gConfig, uint32_t srcId, uint32_t srcPort, 
    uint32_t targetId, uint32_t targetPort)
{
    ConnectConfig* connect = gConfig->add_connects();
    connect->set_src_engine_id(srcId);
    connect->set_src_port_id(srcPort);
    connect->set_target_engine_id(targetId);
    connect->set_target_port_id(targetPort);
}
