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
#include "DynamicGraph.h"
#include <atomic>
#include <libgen.h>
#include <tuple>
#include <unistd.h>

// flag to guard stop signal
static std::atomic<int> g_flag = { 0 };
// sleep time
static const uint32_t USLEEP_TIME = 100000;
// device id
static const uint32_t DEVICE_ID = 0;

HIAI_StatusT CustomDataRecvInterface::RecvData(const std::shared_ptr<void>& message)
{
    std::shared_ptr<std::string> data = std::static_pointer_cast<std::string>(message);
    printf("%s\n", data->c_str());
    g_flag--;
    return HIAI_OK;
}

HIAI_StatusT CreateDynamicGraphs(uint32_t num, dg::DynamicGraph& graphs, std::vector<dg::NodeInfo>& inputNodes,
    std::vector<dg::NodeInfo>& outputNodes)
{
    // init id number
    int id = 100;
    // create graphs dynamically instead of using graph.config file
    for (int i = 0; i < num; i++) {
        dg::graph g(id++, DEVICE_ID);
        dg::engine e0("GreetEngine", id++, 1, dg::engine::HOST);
        dg::engine e1("GreetEngine", id++, 1, dg::engine::DEVICE);
        e1.so_name.push_back("libDevice.so");
        dg::engine e2("GreetEngine", id++, 1, dg::engine::HOST);
        inputNodes.push_back(std::make_tuple(g, e0, 0));
        outputNodes.push_back(std::make_tuple(g, e2, 0));
        g.addEngine(e0);
        g.addEngine(e1);
        g.addEngine(e2);
        g.addConnection(dg::connection(e0, 0, e1, 0));
        g.addConnection(dg::connection(e1, 0, e2, 0));
        graphs.addGraph(g);
    }
    return HIAI_OK;
}

int main(int argc, char* argv[])
{
    CommandParser options;
    options
        .addOption("-h")
        .addOption("-g", "1");
    options.parseArgs(argc, argv);

    bool help = options.cmdOptionExists("-h");
    int groups = parseStrToInt(options.cmdGetOption("-g"));

    if (groups <= 0) {
        printf("option -g should be greater than or equal to 1.\n");
        showUsage();
        return -1;
    }

    g_flag = groups;
    if (help) {
        showUsage();
        return 0;
    }

    // chdir to executable path
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

    dg::DynamicGraph graphs;
    std::vector<dg::NodeInfo> inputNodes;
    std::vector<dg::NodeInfo> outputNodes;
    HIAI_StatusT ret;
    ret = CreateDynamicGraphs(groups, graphs, inputNodes, outputNodes);
    if (ret != HIAI_OK) {
        printf("CreateDynamicGraphs failed %d\n", ret);
        return -1;
    }
    ret = graphs.createGraph();
    if (ret != HIAI_OK) {
        printf("createGraph failed %d\n", ret);
        return -1;
    }
    // set engine ports and recieve callback  from graph to main
    for (const auto& node : outputNodes) {
        ret = graphs.setDataRecvFunctor(node, std::make_shared<CustomDataRecvInterface>(""));
        if (ret != HIAI_OK) {
            printf("setDataRecvFunctor failed %d\n", ret);
            return -1;
        }
    }
    // send from main to graph
    for (const auto& node : inputNodes) {
        ret = graphs.sendData(node, "string", std::make_shared<std::string>("hello world"));
        if (ret != HIAI_OK) {
            printf("sendData failed %d\n", ret);
            return -1;
        }
    }

    while (g_flag > 0) {
        usleep(USLEEP_TIME);
    }

    graphs.destroyGraph();

    return 0;
}