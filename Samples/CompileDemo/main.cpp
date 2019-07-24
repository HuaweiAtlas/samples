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

#include <cstring>
#include <iostream>
#include <unistd.h>

#include "main.h"
#include "hiaiengine/api.h"
#include "hiaiengine/data_type.h"
#include "hiaiengine/data_type_reg.h"

/************************************/
struct MyselfMessage {
    std::string strInfo;

    MyselfMessage() {
        strInfo = "";
    }
};
template<class Archive>
void serialize(Archive& ar, MyselfMessage& data)
{
    ar(data.strInfo);
}
HIAI_REGISTER_DATA_TYPE("MyselfMessage", MyselfMessage);
/************************************/

bool flag = false;
std::string result_info = "";
HIAI_StatusT CustomDataRecvInterface::RecvData(const std::shared_ptr<void>& message)
{
    std::shared_ptr<std::string> data = std::static_pointer_cast<std::string>(message);
    result_info = *data;
    flag = true;
    return HIAI_OK;
}

// Init and create graph
HIAI_StatusT HIAI_InitAndStartGraph(uint32_t GRAPH_ID, std::string graphConfig, hiai::EnginePortID target_port_config)
{
    // Step1: Global System Initialization before using HIAI Engine
    HIAI_StatusT status = HIAI_Init(0);//Atlas 500 only one core

    // Step2: Create and Start the Graph
    status = hiai::Graph::CreateGraph(graphConfig);
    if (status != HIAI_OK)
    {
        HIAI_ENGINE_LOG(status, "Fail to start graph");
        return status;
    }

    // Step3
    std::shared_ptr<hiai::Graph> graph = nullptr;
    graph = hiai::Graph::GetInstance(GRAPH_ID);
    if (graph == nullptr)
    {
        HIAI_ENGINE_LOG("Fail to get the graph object");
        return status;
    }
    
    graph->SetDataRecvFunctor(target_port_config, std::make_shared<CustomDataRecvInterface>(""));

    return HIAI_OK;
}

int main(int argc, char* argv[])
{
    if (argv[1] == nullptr)
    {
        std::cout << "please input param after program" << std::endl;
        exit(1);
    }
    std::string strInput = strdup(argv[1]);

    HIAI_StatusT ret = HIAI_OK;
    uint32_t graphID = 10000;
    std::string graphConfig = "./graph.config";

    hiai::EnginePortID target_port_config;
    target_port_config.graph_id = graphID;
    target_port_config.engine_id = 102;//define by self
    target_port_config.port_id = 0;

    // 1.create graph
    ret = HIAI_InitAndStartGraph(graphID, graphConfig, target_port_config);
    if (ret != HIAI_OK)
    {
        HIAI_ENGINE_LOG("Fail to start graph");
        return HIAI_ERROR;
    }

    // 2.send data
    std::shared_ptr<hiai::Graph> graph = hiai::Graph::GetInstance(graphID);
    if (graph == nullptr)
    {
        HIAI_ENGINE_LOG("Fail to create graph");
        return HIAI_ERROR;
    }
                             
    // send data to SourceEngine 0 port 
    hiai::EnginePortID src_port_config;
    src_port_config.graph_id = graphID;
    src_port_config.engine_id = 100;
    src_port_config.port_id = 0;
            
    std::shared_ptr<MyselfMessage> sendData_ptr = std::make_shared <MyselfMessage> ();
    sendData_ptr->strInfo = strInput;
    HIAI_ENGINE_LOG("Send data to transfer engine");
    graph->SendData(src_port_config, "MyselfMessage", std::static_pointer_cast<void>(sendData_ptr));
    while (flag == false)
    {
        usleep(100000);
    }
    HIAI_ENGINE_LOG("Get result successfully");
            
    hiai::Graph::DestroyGraph(graphID);
    std::cout << result_info << std::endl;

    HIAI_ENGINE_LOG("Main INFO child exit!");
    return 0;
}
