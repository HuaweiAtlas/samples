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

#ifndef HELLODAVINCI_GRAPH_MANAGER_H
#define HELLODAVINCI_GRAPH_MANAGER_H

#include <thread>
#include "hiaiengine/api.h"
#include <string.h>
#include "hiaiengine/graph.h"
#include <atomic>
#include <vector>

using namespace std;
using namespace hiai;

class GraphManager : public hiai::DataRecvInterface {
public:
    explicit GraphManager(const vector<uint32_t> graphIdArray, const vector<uint32_t> chipIdArray,
                          const vector<string> graphPathArray) :
        graphIdArray(graphIdArray),
        chipIdArray(chipIdArray), graphPathArray(graphPathArray), g_sendFlag(0),
        isRunning(false)
    {

    }

    /**
     * @brief StartGraph, start graph
     * @return : HIAI_StatusT, HIAI_OK: success, HIAI_ERROR: fail
     */
    HIAI_StatusT StartGraph();

    /**
     * @brief InitChip, init chip
     * @return : HIAI_StatusT, HIAI_OK: success, HIAI_ERROR: fail
     */
    HIAI_StatusT InitChip();

    /**
     * @brief get graph running status
     * @return : true: is running, false: is not running
     */
    bool IsRunning();

    /**
     * @brief SendData, send data to dst engine
     * @param [in] : graphId, the id of  graph
     * @param [in] : engineId, the id of dst engine
     * @param [in] : structType, the type of data to send
     * @param [in] : structData , the data to send
     * @return : HIAI_StatusT, HIAI_OK: success, HIAI_ERROR: fail
     */
    HIAI_StatusT SendData(const uint32_t graphId, const uint32_t engineId, const std::string &structType,
                          const std::shared_ptr<void> &structData);

    /**
     * @brief Destroy all Graph
     * @return : HIAI_StatusT, HIAI_OK: success, HIAI_ERROR: fail
     */
    HIAI_StatusT DestroyAllGraph();

    /**
     * @brief register the callback function
     * @param [in] : dataRecv, DataRecvInterface where definite callback function
     * @param [in] : graphId, the id of graph
     * @param [in] : dstEngineId, the id of dst engine where send data back to main
     * @return : HIAI_StatusT, HIAI_OK: success
     */
    HIAI_StatusT SetAllGraphRecvFunctor(const std::shared_ptr<DataRecvInterface> &dataRecv, const uint32_t graphId,
                                        const uint32_t dstEngineId);

    /**
     * @brief  it would be called when receiving message
     * @return : HIAI_StatusT, HIAI_OK: success
     */
    HIAI_StatusT RecvData(const std::shared_ptr<void> &message);

private:
    vector<uint32_t> chipIdArray;
    bool isRunning;
    vector<string> graphPathArray;
    const vector<uint32_t> graphIdArray;
    /* data signal. +1 when send data to next engine sucessfully, -1 when receive data from Save file engine. */
    atomic_int g_sendFlag;
};

#endif  // HELLODAVINCI_GRAPH_MANAGER_H
