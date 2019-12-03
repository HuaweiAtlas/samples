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

#include <cstdlib>
#include <hiaiengine/api.h>
#include <cstring>
#include <libgen.h>
#include "hiaiengine/api.h"
#include "CommandLine.h"
#include "CommandParser.h"
#include "AppCommon.h"
#include "Common.h"
#include "hiaiengine/ai_memory.h"
#include "TransSear.h"
#include "GraphManager.h"
#include "FileManager.h"
#include <securec.h>
#include <regex>
#include "EngineImageTrans.h"

using namespace std;
using namespace hiai;

HIAI_REGISTER_SERIALIZE_FUNC("GraphCtrlInfoT", GraphCtrlInfoT, GetTransSearPtr, GetTransDearPtr);
// register EngineImageTransT
HIAI_REGISTER_SERIALIZE_FUNC("EngineImageTransT", EngineImageTransT, GetEngineImageTransPtr,
                             GetEngineImageTransrPtr);

// The following constants are parameters for graph
// and must be consistent with the corresponding fields in the configuration file graph.config.
static const uint32_t GRAPH_ID = 437647942;
// chip id.
static const uint32_t CHIP_ID = 0;
static const uint32_t SRC_ENGINE_ID = 470;      // the id of input engine
static const uint32_t DSTENGINE_ID = 801;       // the if of dst engine
static const uint32_t DELAY_USECONDS = 100000;  // time to sleep
static const uint32_t DEMALLOC_TIMEOUT = 10000;
static const string GRAPH_CONFIG_PATH = "./graph.config";  // config file path
static const string DEFAULT_INPUT = "../data/test.jpeg";
static const string JPEG_EXTENSION = "jpeg";
static const string JPG_EXTENSION = "jpg";
static const uint32_t BUFFER_LEN_OFFSET = 8;

HIAI_StatusT HiaiSendDataToDevice(shared_ptr<GraphManager> graphManager, const std::string &filePath)
{
    string inputFile = filePath;

    shared_ptr<FileManager> fileManager(new FileManager());

    FileInfo imageFileInfo = FileInfo();

    bool readRet = fileManager->ReadFileWithDmalloc(inputFile, imageFileInfo);
    if (!readRet) {
        printf("Read ImageFile error.");
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Read ImageFile error.");
        return false;
    }

    uint32_t fileLen = imageFileInfo.size;
    uint32_t bufferLen = fileLen + BUFFER_LEN_OFFSET;

    std::shared_ptr<GraphCtrlInfoT> ctrlInfo = make_shared<GraphCtrlInfoT>();
    ctrlInfo->bufferSize = bufferLen;

    ctrlInfo->dataBuff = imageFileInfo.data;
    HIAI_StatusT getRet = graphManager->SendData(GRAPH_ID, SRC_ENGINE_ID, "GraphCtrlInfoT",
                                                 static_pointer_cast<void>(ctrlInfo));
    if (getRet != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Send data to device failed!");
        return HIAI_ERROR;
    }

    return HIAI_OK;
}

/**
 * @brief This function show a help message
 * @param [in] help, show help message
 * @param [in] filePath, image path
 */
bool checkArgs(bool help, const std::string &filePath)
{
    if (help) {
        ShowUsage();
        return false;
    }
    char path[PATH_MAX + 1] = { 0x00 };
    errno_t err = strcpy_s(path, PATH_MAX + 1, filePath.c_str());
    if (err != EOK) {
        printf("[ERROR] strcpy %s failed!\n", path);
        return false;
    }

    if (-1 == access(path, R_OK)) {
        printf("[ERROR] Input file-- %s doesn't exit!\n", path);
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[ERROR] Input file-- %s doesn't exit!", path);
        return false;
    }

    return true;
}

bool ParseAndCheckArgs(int argc, char *argv[], std::string &filePath)
{
    CommandParser options;
    options.addOption(g_HELP_CMD).addOption(g_INPUT_CMD, DEFAULT_INPUT);

    options.parseArgs(argc, argv);

    bool help = options.cmdOptionExists(g_HELP_CMD);
    filePath = options.cmdGetOption(g_INPUT_CMD);
    if (!checkArgs(help, filePath)) {
        return false;
    }
    return true;
}

// get the file extension
string GetImageFormat(const string &filePath)
{
    std::set<char> delims { '.' };
    std::vector<std::string> path = splitpath(filePath, delims);
    return path[path.size() - 1];
}

int main(int argc, char *argv[])
{
    printf("[main] The sample begins!\n");
    // change to executable file directory
    shared_ptr<FileManager> fileManager(new FileManager());

    string path(argv[0], argv[0] + strlen(argv[0]));
    fileManager->ChangeDir(path.c_str());

    // Parse input and check param
    string filePath = "";
    if (!ParseAndCheckArgs(argc, argv, filePath)) {
        return false;
    }

    string imageFormat = GetImageFormat(filePath);
    if ((JPEG_EXTENSION != imageFormat) && (JPG_EXTENSION != imageFormat)) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[ERROR] this sample only support jpeg format.");
        return -1;
    }

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

    // 3.send data to device
    ret = HiaiSendDataToDevice(graphManager, filePath);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Fail to get send data. ret = %d", ret);
        return -1;
    }

    // delay in main if graph is running
    while (graphManager->IsRunning()) {
        usleep(DELAY_USECONDS);
    }

    // destroy graph
    ret = graphManager->DestroyAllGraph();
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Fail to destroy graph, program exit. ret = %d", ret);
        return -1;
    } else {
        HIAI_ENGINE_LOG(HIAI_IDE_INFO, "Success to to destroy graph, program exit.");
    }
    printf("[main] The sample ends!\n");

    return 0;
}
