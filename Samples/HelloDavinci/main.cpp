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

#include <unistd.h>
#include <vector>
#include <libgen.h>
#include <string>
#include "hiaiengine/api.h"
#include "hiaiengine/graph.h"
#include "GraphManager.h"
#include "FileManager.h"
#include "AppCommon.h"
#include "Common.h"

using namespace std;
using namespace hiai;

// The following constants are parameters for graph
// and must be consistent with the corresponding fields in the configuration file graph.config.
static const uint32_t GRAPH_ID = 100;
// chip id.
static const uint32_t CHIP_ID = 0;
static const uint32_t SRC_ENGINE_ID = 100;                 // the id of input engine
static const uint32_t DSTENGINE_ID = 300;                  // the if of dst engine
static const uint32_t DELAY_USECONDS = 100000;             // time to sleep
static const string GRAPH_CONFIG_PATH = "./graph.config";  // config file path

int main(int argc, char *argv[])
{
    // change to executable file directory
    shared_ptr<FileManager> fileManager(new FileManager());
    fileManager->ChangeDir(argv[0]);

    vector<uint32_t> chipIdArray { CHIP_ID };
    vector<string> graphConfigPath { GRAPH_CONFIG_PATH };
    vector<uint32_t> graphIdArray { GRAPH_ID };
    shared_ptr<GraphManager> graphManager(new GraphManager(graphIdArray, chipIdArray, graphConfigPath));

    // 1.init chip
    HIAI_StatusT ret = graphManager->InitChip();
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Fail to init chip. ret = %d", ret);
        return -1;
    }

    // 2.create graph
    ret = graphManager->StartGraph();
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Fail to start graph. ret = %d", ret);
        return -1;
    }
    // Register the DstEngine
    ret = graphManager->SetAllGraphRecvFunctor(graphManager, GRAPH_ID, DSTENGINE_ID);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Fail to register the DstEngine. ret = %d", ret);
        return -1;
    }

    // 3.send data
    shared_ptr<string> strData(new string);
    ret = graphManager->SendData(GRAPH_ID, SRC_ENGINE_ID, "string", static_pointer_cast<void>(strData));
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Fail to get send data. ret = %d", ret);
        return -1;
    }

    // delay in main if graph is running
    while (graphManager->IsRunning()) {
        usleep(DELAY_USECONDS);
    }

    printf("Hello Davinci!\n");
    // destroy graph
    ret = graphManager->DestroyAllGraph();
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Fail to destroy graph, program exit. ret = %d", ret);
        return -1;
    } else {
        HIAI_ENGINE_LOG(HIAI_IDE_INFO, "Success to to destroy graph, program exit.");
    }
    printf("The sample end!!\n");

    return 0;
}
