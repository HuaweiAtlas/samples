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

#include "main.h"
#include <unistd.h>
#include <libgen.h>
#include <cstring>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>
#include "hiaiengine/api.h"
#include "hiaiengine/data_type.h"
#include "hiaiengine/data_type_reg.h"
#include "FileManager.h"
#include "Common.h"
#include "DataType.h"

const uint32_t GRAPH_ID = 442;
const uint32_t USLEEP_TIME = 100000;
const uint32_t TARGET_ENGINE_ID = 203;
const uint32_t SRC_ENGINE_ID = 673;
static bool g_flag = false;
std::string g_resultInfo = "";
static const char HELP_MESSAGE[] = "Print a usage message.";
static const char I_MESSAGE[] = "Requested. Specify the input image, eg, /home/data/test.yuv";
static const char HEIGHT_MESSAGE[] = "Requested. Specify the height of input image range = [128, 1920]";
static const char WIDTH_MESSAGE[] = "Requested. Specify the width of input image range = [128, 1920]";

HIAI_REGISTER_DATA_TYPE("EncodeVideoBlock", EncodeVideoBlock);
/**
 * @ingroup FasterRcnnDataRecvInterface
 * @brief RecvData 
 */
HIAI_StatusT CustomDataRecvInterface::RecvData(const std::shared_ptr<void> &message)
{
    std::shared_ptr<std::string> data = std::static_pointer_cast<std::string>(message);
    g_resultInfo = *data;
    g_flag = true;

    return HIAI_OK;
}

// Init and create graph
HIAI_StatusT HIAI_InitAndStartGraph(int chipIndex, uint32_t GRAPH_ID, std::string graphConfig,
                                    hiai::EnginePortID target_port_config)
{
    // Step1: Global System Initialization before using HIAI Engine
    HIAI_StatusT status = HIAI_Init(chipIndex);
    if (status != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Failed to start the graph");
        return status;
    }
    // Step2: Create and Start the Graph
    status = hiai::Graph::CreateGraph(graphConfig);
    if (status != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Failed to start the graph");
        return status;
    }

    // Step3: Get the Graph
    std::shared_ptr<hiai::Graph> graph = hiai::Graph::GetInstance(GRAPH_ID);
    if (graph == nullptr) {
        HIAI_ENGINE_LOG("Failed to get the graph-%u", GRAPH_ID);
        return status;
    }

    graph->SetDataRecvFunctor(target_port_config,
                              std::shared_ptr<CustomDataRecvInterface>(new CustomDataRecvInterface("")));

    return HIAI_OK;
}

static HIAI_StatusT CheckArgs(const std::string& fileName, const int height, const int width)
{
    // check if the file exist
    shared_ptr<FileManager> fileManager(new FileManager());
    if (fileManager->ExistFile(fileName) == false) {
        printf("[ERROR] Input file %s doesn't exist or It is a directory. Please check!\n", fileName.c_str());
        return HIAI_ERROR;
    }
    if (height <= 0 || width <= 0) {
        printf("[ERROR] the height or width is not valid or given, please check the help message\n");
        return HIAI_ERROR;
    }
    
    return HIAI_OK;
}

static void ShowUsage()
{
    printf("\nUsage: dvppTest [Options...]\n\n");
    printf("Options:\n");
    printf("    -h                             %s\n", HELP_MESSAGE);
    printf("    -i '<path>'                    %s\n", I_MESSAGE);
    printf("    -height                        %s\n", HEIGHT_MESSAGE);
    printf("    -width                         %s\n", WIDTH_MESSAGE);
}

int SetPort(uint32_t graphID, hiai::EnginePortID& srcPortConfig, hiai::EnginePortID& targetPortConfig)
{
    srcPortConfig.graph_id = graphID;
    srcPortConfig.engine_id = SRC_ENGINE_ID;
    srcPortConfig.port_id = 0;

    targetPortConfig.graph_id = graphID;
    targetPortConfig.engine_id = TARGET_ENGINE_ID;
    targetPortConfig.port_id = 0;

    return 0;
}

int main(int argc, char *argv[])
{
    CommandParser options;
    options.addOption("-h").addOption("-i", "../data/test.yuv").addOption("-height", "0").addOption("-width", "0");
    options.parseArgs(argc, argv);
    bool help = options.cmdOptionExists("-h");
    std::string fileName = options.cmdGetOption("-i");
    int height = parseStrToInt(options.cmdGetOption("-height"));
    int width = parseStrToInt(options.cmdGetOption("-width"));
    // check the validity of input argument
    if (help || CheckArgs(fileName, height, width) != HIAI_OK) {
        ShowUsage();
        return -1;
    }
    shared_ptr<FileManager> fileManager(new FileManager());
    string path(argv[0], argv[0] + strlen(argv[0]));
    fileManager->ChangeDir(path.c_str());

    int chipIndex = 0;
    uint32_t graphID = GRAPH_ID;
    std::string graphConfig = "./graph.config";
    
    hiai::EnginePortID targetPortConfig;
    hiai::EnginePortID srcPortConfig;
    SetPort(graphID, srcPortConfig, targetPortConfig);

    // 1.create graph
    HIAI_StatusT ret = HIAI_InitAndStartGraph(chipIndex, graphID, graphConfig, targetPortConfig);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Failed to start graph");
        return HIAI_ERROR;
    }

    // 2.send data
    std::shared_ptr<hiai::Graph> graph = hiai::Graph::GetInstance(graphID);
    if (graph == nullptr) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Failed to get the graph-%u", graphID);
        return HIAI_ERROR;
    }

    // send data to SourceEngine 0 port
    std::shared_ptr<EncodeVideoBlock> sendDataPtr = std::make_shared<EncodeVideoBlock>();
    sendDataPtr->rawImageFile = fileName;
    sendDataPtr->frameId = 0;
    sendDataPtr->isEOS = 0;
    sendDataPtr->imageHeight = height;
    sendDataPtr->imageWidth = width;
    if (HIAI_OK != graph->SendData(srcPortConfig, "EncodeVideoBlock", std::static_pointer_cast<void>(sendDataPtr))) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[main] graph->SendData() error!");
        return HIAI_ERROR;
    }

    while (!g_flag) {
        usleep(USLEEP_TIME);
    }
    hiai::Graph::DestroyGraph(graphID);
    std::cout << "Video Encode success!" << std::endl;

    return 0;
}
