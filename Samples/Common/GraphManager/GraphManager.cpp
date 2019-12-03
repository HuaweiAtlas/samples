/**
 * ============================================================================
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Atlas Sample
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

#include "GraphManager.h"

/**
 * @brief InitChip, init chip
 * @return : HIAI_StatusT, HIAI_OK: success, HIAI_ERROR: fail
 */
HIAI_StatusT GraphManager::InitChip()
{
    for (int i = 0; i < chipIdArray.size(); i++) {
        HIAI_StatusT ret = HIAI_Init(chipIdArray[i]);
        if (ret != HIAI_OK) {
            return HIAI_ERROR;
        }
    }
    return HIAI_OK;
}

/**
 * @brief StartGraph, start graph
 * @return : HIAI_StatusT, HIAI_OK: success, HIAI_ERROR: fail
 */
HIAI_StatusT GraphManager::StartGraph()
{
    for (int i = 0; i < graphPathArray.size(); i++) {
        HIAI_StatusT ret = Graph::CreateGraph(graphPathArray[i]);
        if (ret != HIAI_OK) {
            return HIAI_ERROR;
        }
    }
    return HIAI_OK;
}

/**
 * @brief register the callback function
 * @param [in] : dataRecv, DataRecvInterface where definite callback function
 * @param [in] : graphId, the id of graph
 * @param [in] : dstEngineId, the id of dst engine where send data back to main
 * @return : HIAI_StatusT, HIAI_OK: sucess
 */
HIAI_StatusT GraphManager::SetAllGraphRecvFunctor(const std::shared_ptr<DataRecvInterface> &dataRecv,
                                                  const uint32_t graphId,
                                                  const uint32_t dstEngineId)
{
    EnginePortID targetPort;
    targetPort.graph_id = graphId;
    targetPort.engine_id = dstEngineId;
    targetPort.port_id = 0;
    std::shared_ptr<Graph> graph = Graph::GetInstance(graphId);

    return graph->SetDataRecvFunctor(targetPort, dataRecv);
}

/**
 * @brief  it would be called when receiving message
 * @return : HIAI_StatusT, HIAI_OK: success
 */
HIAI_StatusT GraphManager::RecvData(const std::shared_ptr<void> &message)
{
    std::shared_ptr<std::string> data =
        std::static_pointer_cast<std::string>(message);
    g_sendFlag--;
    if (g_sendFlag <= 0) {
        this->isRunning = false;
    }
    return HIAI_OK;
}

/**
 * @brief Destroy all Graph
 * @return : HIAI_StatusT, HIAI_OK: success, HIAI_ERROR: fail
 */
HIAI_StatusT GraphManager::DestroyAllGraph()
{
    for (int i = 0; i < graphIdArray.size(); i++) {
        HIAI_StatusT ret = Graph::DestroyGraph(graphIdArray[i]);
        if (ret != HIAI_OK) {
            return HIAI_ERROR;
        }
    }
    return HIAI_OK;
}

/**
 * @brief SendData, send data to dst engine
 * @param [in] : graphId, the id of  graph
 * @param [in] : engineId, the id of dst engine
 * @param [in] : structType, the type of data to send
 * @param [in] : structData , the data to send
 * @return : HIAI_StatusT, HIAI_OK: success, HIAI_ERROR: fail
 */
HIAI_StatusT GraphManager::SendData(const uint32_t graphId, const uint32_t engineId, const std::string &structType,
                                    const std::shared_ptr<void> &structData)
{
    this->isRunning = true;
    EnginePortID enginePortID;
    enginePortID.graph_id = graphId;
    enginePortID.engine_id = engineId;
    enginePortID.port_id = 0;
    std::shared_ptr<Graph> graph = Graph::GetInstance(graphId);

    HIAI_StatusT status = graph->SendData(enginePortID, structType, structData);
    if (status == HIAI_OK) {
        g_sendFlag++;
    } else {
        return HIAI_ERROR;
    }
    return HIAI_OK;
}

/**
 * @brief get graph running status
 * @return : true: is running, false: is not running
 */
bool GraphManager::IsRunning()
{
    return this->isRunning;
}

GraphManager::~GraphManager()
{
    
}

