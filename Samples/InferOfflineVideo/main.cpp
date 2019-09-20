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
#include "DataRecv.h"
#include "hiaiengine/api.h"
#include <atomic>
#include <iostream>
#include <libgen.h>
#include <unistd.h>

static const std::string GRAPH_FILENAME = "./graph.config";
// graph id
static const uint32_t GRAPH_ID = 100;
// destination engine id
static const uint32_t SRC_ENGINE = 101;
// leaf node engine id
static const uint32_t terminators[] = { 106 };
// flag to guard eos signal
static std::atomic<int> g_flag = { 1 };

HIAI_StatusT CustomDataRecvInterface::RecvData(const std::shared_ptr<void>& message)
{
    std::shared_ptr<std::string> data = std::static_pointer_cast<std::string>(message);
    std::cout << "RecvData g_flag " << g_flag << "\n";
    g_flag--;
    return HIAI_OK;
}

// Init and create graph
HIAI_StatusT HIAI_InitAndStartGraph(const std::string& configFile)
{
    // Step1: Global System Initialization before using HIAI Engine
    HIAI_StatusT status = HIAI_Init(0);

    // Step2: Create and Start the Graph
    status = hiai::Graph::CreateGraph(configFile);
    if (status != HIAI_OK) {
        HIAI_ENGINE_LOG(status, "Fail to start graph");
        return status;
    }

    // Step3: Set DataRecv Functor
    std::shared_ptr<hiai::Graph> graph = hiai::Graph::GetInstance(GRAPH_ID);
    if (graph == nullptr) {
        HIAI_ENGINE_LOG("Fail to get the graph-%u", GRAPH_ID);
        return status;
    }

    for (int i = 0; i < sizeof(terminators) / sizeof(uint32_t); i++) {
        hiai::EnginePortID target_port_config;
        target_port_config.graph_id = GRAPH_ID;
        target_port_config.engine_id = terminators[i];
        target_port_config.port_id = 0;
        graph->SetDataRecvFunctor(target_port_config,
            std::make_shared<CustomDataRecvInterface>(""));
    }
    return HIAI_OK;
}

void my_handler(int s)
{
    printf("Caught signal %d\n", s);
    if (s == 2) {
        printf("DestroyGraph %u\n", GRAPH_ID);
        hiai::Graph::DestroyGraph(GRAPH_ID);
        exit(0);
    }
}

int main(int argc, char* argv[])
{
    // cd to directory of main
    char* dirc = strdup(argv[0]);
    if (dirc != NULL) {
        char* dname = ::dirname(dirc);
        int r = chdir(dname);
        if (r != 0) {
            printf("chdir error code %d\n", r);
            return -1;
        }
        free(dirc);
    }

    // init Graph
    HIAI_StatusT ret = HIAI_InitAndStartGraph(GRAPH_FILENAME);
    if (ret != HIAI_OK) {
        printf("[main] Fail to start graph\n");
        return -1;
    }

    // send data
    std::shared_ptr<hiai::Graph> graph = hiai::Graph::GetInstance(GRAPH_ID);
    if (nullptr == graph) {
        printf("Fail to get the graph-%u\n", GRAPH_ID);
        return -1;
    }
    hiai::EnginePortID engine_id;
    engine_id.graph_id = GRAPH_ID;
    engine_id.engine_id = SRC_ENGINE;
    engine_id.port_id = 0;
    std::shared_ptr<std::string> src_data(new std::string());

    graph->SendData(engine_id, "string", std::static_pointer_cast<void>(src_data));

    // wait for ctrl+c
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    while (g_flag > 0) {
        usleep(10000);
    }

    // end
    hiai::Graph::DestroyGraph(GRAPH_ID);
    printf("[main] destroy graph-%u done\n", GRAPH_ID);
    return 0;
}