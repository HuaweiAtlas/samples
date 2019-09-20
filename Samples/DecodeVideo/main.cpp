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
#include "CommandLine.h"
#include "CommandParser.h"
#include "CustomDataRecvInterface.h"
#include "hiaiengine/api.h"
#include "proto/graph_config.pb.h"
#include <algorithm>
#include <atomic>
#include <cstring>
#include <libgen.h>
#include <unistd.h>

// flag to guard eos signal
static std::atomic<int> g_flag = { 0 };
// graph id to run
static std::vector<uint32_t> g_graphIds;
// graph config file
static const std::string GRAPH_FILENAME = "./graph.config";
// source engine id
static const uint32_t SRC_ENGINE = 100;
// leaf node engine id
static const uint32_t TERMINATORS[] = { 300 };
// send data port id
static const uint32_t PORT_ID = 0;
// graph id
static const uint32_t GRAPH_ID = 100;
// sleep time
static const uint32_t USLEEP_TIME = 100000;
/**
 * @ingroup CustomDataRecvInterface
 * @brief RecvData callback function
 * @param [in]
 */
HIAI_StatusT CustomDataRecvInterface::RecvData(const std::shared_ptr<void>& message)
{
    std::shared_ptr<std::string> data = std::static_pointer_cast<std::string>(message);
    // std::cout << "RecvData g_flag " << g_flag << "\n";
    g_flag--;
    return HIAI_OK;
}

// Init and create graph
HIAI_StatusT HIAI_InitAndStartGraph(const std::vector<uint32_t>& graphs2run)
{
    // args check
    if (graphs2run.size() < 1) {
        printf("[Error] Invalid input graph number %zu\n", graphs2run.size());
        return HIAI_ERROR;
    }
    // Step1: Global System Initialization before using HIAI Engine
    HIAI_StatusT status = HIAI_Init(1);
    hiai::GraphConfigList glist;
    hiai::Graph::ParseConfigFile(GRAPH_FILENAME, glist);
    // append graphs
    int gsize = glist.graphs_size();
    if (1 != gsize) {
        printf("[Error] Unexpected graph list size %u\n", gsize);
        return HIAI_ERROR;
    }

    hiai::GraphConfig* graph0 = glist.mutable_graphs(0);
    printf("[HIAI_InitAndStartGraph] number of graph to run %zu\n", graphs2run.size());
    for (int i = 0; i < graphs2run.size(); i++) {
        hiai::GraphConfig* eachGraph = NULL;
        if (i == 0) {
            eachGraph = graph0;
        } else {
            eachGraph = glist.add_graphs();
            eachGraph->CopyFrom(*graph0);
        }
        int graphId = (i + 1) * GRAPH_ID;
        g_graphIds.push_back(graphId);
        eachGraph->set_graph_id(graphId);
        if (graphs2run[i] == 0) {
            eachGraph->mutable_engines(0)
                ->mutable_ai_config()
                ->mutable_items(0)
                ->set_value("h264");
            printf("[HIAI_InitAndStartGraph] add h264 graph\n");
        } else {
            eachGraph->mutable_engines(0)
                ->mutable_ai_config()
                ->mutable_items(0)
                ->set_value("h265");
            printf("[HIAI_InitAndStartGraph] add h265 graph\n");
        }
        eachGraph->mutable_engines(0)
            ->mutable_ai_config()
            ->mutable_items(1)
            ->set_value(std::to_string(i));
    }
    // Step2: Create and Start the Graph
    status = hiai::Graph::CreateGraph(glist);
    if (status != HIAI_OK) {
        printf("[Error] Fail to start graph\n");
        return status;
    }
    for (int g = 0; g < g_graphIds.size(); g++) {
        // Step3
        std::shared_ptr<hiai::Graph> graph = hiai::Graph::GetInstance(g_graphIds[g]);
        if (nullptr == graph) {
            printf("[Error] Fail to get the graph-%u\n", g_graphIds[g]);
            return status;
        }

        for (int i = 0; i < sizeof(TERMINATORS) / sizeof(uint32_t); i++) {
            hiai::EnginePortID target_port_config;
            target_port_config.graph_id = g_graphIds[g];
            target_port_config.engine_id = TERMINATORS[i];
            target_port_config.port_id = PORT_ID;
            graph->SetDataRecvFunctor(target_port_config,
                std::shared_ptr<CustomDataRecvInterface>(new CustomDataRecvInterface("")));
        }
    }
    return HIAI_OK;
}

int main(int argc, char* argv[])
{
    std::vector<uint32_t> graphs2run;

    CommandParser options;

    options
        .addOption("-h")
        .addOption("-g", "1")
        .addOption("-d", "0")
        .addOption("-i", "../data/test.h264");

    options.parseArgs(argc, argv);

    bool help = options.cmdOptionExists("-h");
    std::string filename = options.cmdGetOption("-i");
    int decode = parseStrToInt(options.cmdGetOption("-d"));
    int groups = parseStrToInt(options.cmdGetOption("-g"));

    char* dirc = strdup(argv[0]);
    if (dirc != NULL) {
        char* dname = ::dirname(dirc);
        int r = chdir(dname);
        free(dirc);
    }

    if (help || HIAI_OK != checkArgs(filename, decode, groups)) {
        showUsage();
        return -1;
    }

    for (int i = 0; i < groups; i++) {
        graphs2run.push_back(decode);
    }

    // 1.create graph
    HIAI_StatusT ret = HIAI_InitAndStartGraph(graphs2run);
    if (ret != HIAI_OK) {
        printf("[main] Fail to start graph\n");
        return -1;
    }
    for (int g = 0; g < g_graphIds.size(); g++) {
        // 2.send data
        std::shared_ptr<hiai::Graph> graph = hiai::Graph::GetInstance(g_graphIds[g]);
        if (nullptr == graph) {
            HIAI_ENGINE_LOG("Fail to get the graph-%u", g_graphIds[g]);
            return -1;
        }
        // send data to SourceEngine 0 port
        hiai::EnginePortID engine_id;
        engine_id.graph_id = g_graphIds[g];
        engine_id.engine_id = SRC_ENGINE;
        engine_id.port_id = 0;
        std::shared_ptr<std::string> src_data(new std::string(filename));
        g_flag++;
        HIAI_StatusT ret = graph->SendData(engine_id, "string", std::static_pointer_cast<void>(src_data));
        if (ret != HIAI_OK) {
            g_flag--;
        }
    }

    while (g_flag > 0) {
        usleep(USLEEP_TIME);
    }

    printf("[main] the demo run success!\n");
    for (const auto& gid : g_graphIds) {
        hiai::Graph::DestroyGraph(gid);
    }
    return 0;
}
