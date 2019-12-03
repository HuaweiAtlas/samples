/**
 *
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
 *     contributors may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * ND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
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

#include "hiaiengine/api.h"
#include "CommandParser.h"
#include "CommandLine.h"
#include "GraphManager.h"
#include "TransSear.h"
#include "Common.h"
#include "FileManager.h"

#define MS(x) ((x)*1000)
// The following constants are parameters for graph
// and must be consistent with the corresponding fields in the configuration file graph.config.
static const uint32_t GRAPH_ID = 100;
// chip id.
static const uint32_t CHIP_ID = 0;
static const uint32_t SRC_ENGINE_ID = 1000;                // the id of input engine
static const uint32_t DSTENGINE_ID = 2000;                 // the if of dst engine
static const uint32_t DELAY_USECONDS = 100000;             // time to sleep
static const string GRAPH_CONFIG_PATH = "./graph.config";  // config file path

/*
 * function: initialize the graph manager module
 * params: graphManager, the pointer of an instance of graph manager
 * return: HIAI_StatusT, success or not
 */
HIAI_StatusT InitGraphManager(std::shared_ptr<GraphManager> &graphManager)
{
    HIAI_StatusT ret;
    ret = graphManager->InitChip();  // initialize the chip.
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Failed to init chip");
        return HIAI_ERROR;
    }
    // 1. Create graph
    ret = graphManager->StartGraph();
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG("Fail to start graph");
        return HIAI_ERROR;
    }
    // 2. Set the callback function of received data
    ret = graphManager->SetAllGraphRecvFunctor(graphManager, GRAPH_ID, DSTENGINE_ID);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Failed to register the DstEngine");
        return HIAI_ERROR;
    }
    return HIAI_OK;
}

int main(int argc, char *argv[])
{
    shared_ptr<FileManager> fileManager(new FileManager());
    string path(argv[0], argv[0] + strlen(argv[0]));
    fileManager->ChangeDir(path.c_str());

    // set the options of CommandParser
    CommandParser options;
    options.addOption("-h").addOption("-i", "../data/test.yuv").addOption("-height",
            "0").addOption("-width", "0").addOption("-format", "-1");

    options.parseArgs(argc, argv);
    bool help = options.cmdOptionExists("-h");
    std::string fileName = options.cmdGetOption("-i");
    int height = parseStrToInt(options.cmdGetOption("-height"));
    int width = parseStrToInt(options.cmdGetOption("-width"));
    int format = parseStrToInt(options.cmdGetOption("-format"));
    // check the validity of input argument
    if (help || HIAI_OK != CheckArgs(fileName, height, width, format)) {
        ShowUsage();
        return -1;
    }
    // get the absolute path of input file
    char absPath[PATH_MAX];
    if (realpath(fileName.c_str(), absPath) == NULL) {
        printf("Get the real path failed!\n");
        return -1;
    }

    // initialize the instance of graph manager
    std::vector<uint32_t> chipIdArray { CHIP_ID };
    std::vector<std::string> graphConfigPath { GRAPH_CONFIG_PATH };
    std::vector<uint32_t> graphIdArray { GRAPH_ID };
    std::shared_ptr<GraphManager> graphManager(new GraphManager(graphIdArray, chipIdArray, graphConfigPath));
    HIAI_StatusT ret;
    ret = InitGraphManager(graphManager);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Failed to Init GraphManager");
        return -1;
    }
    // construct the srcData with the input argument
    std::shared_ptr<YUVInfoT> srcData = std::make_shared<YUVInfoT>();
    srcData->fileName = std::string(absPath);
    srcData->height = height;
    srcData->width = width;
    srcData->format = format;
    ret = graphManager->SendData(GRAPH_ID, SRC_ENGINE_ID, "YUVInfoT", std::static_pointer_cast<void>(srcData));
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Failed to send data");
        return -1;
    }

    while (graphManager->IsRunning()) {
        usleep(DELAY_USECONDS);
    }
    // destroy the graph before exit
    ret = graphManager->DestroyAllGraph();
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Failed to destroy graph");
        return -1;
    } else {
        HIAI_ENGINE_LOG(HIAI_IDE_INFO, "Success to destroy graph");
    }
    return 0;
}

