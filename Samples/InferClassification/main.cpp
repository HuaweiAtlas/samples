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
#include <hiaiengine/api.h>
#include <libgen.h>
#include <string>
#include "CommandLine.h"
#include "CommandParser.h"
#include "AppCommon.h"
#include "Common.h"
#include "hiaiengine/ai_memory.h"
#include "GraphManager.h"
#include "FileManager.h"

using namespace std;

// The following constants are parameters for graph
// and must be consistent with the corresponding fields in the configuration file graph.config.
static const uint32_t GRAPH_ID = 1000;  // the id of a graph
static const uint32_t SRC_ENGINE_ID = 101;   // the id of input engine
static const uint32_t DSTENGINE_ID = 106;    // the id of lead node
// chip id.
static const uint32_t CHIP_ID = 0;
static const string GRAPH_CONFIG_PATH = "./graph.config";  // config file path
static const string DEFAULT_INPUT = "../data/test.jpeg";
static const uint32_t DELAY_USECONDS = 100000;             // time to sleep
static const uint32_t IS_DIR = 1;
static const uint32_t IS_FILE = 0;

bool CheckArgs(const bool help, const std::string &filePath, const uint32_t decodeType, const uint32_t dirType)
{
    if (help) {
        ShowUsage();
        return false;
    }

    if ((decodeType != DEFAULT_IMAGE_TYPE) && (decodeType != TYPE_JPEG) && (decodeType != TYPE_PNG)) {
        printf("[ERROR] Dvpp type-- %d is invalid! 0: jpeg, 1: png.\n", decodeType);
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[ERROR] Dvpp type-- %d is invalid! 0: jpeg, 1: png.", decodeType);
        return false;
    }

    shared_ptr<FileManager> fileManager(new FileManager());
    if (dirType == IS_FILE) {
        if (fileManager->ExistFile(filePath) == false) {
            printf("[ERROR] Input isn't a file!\n");
            HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[ERROR] Input file %s isn't a file or not exist!", filePath.c_str());
            return false;
        }
    } else if (dirType == IS_DIR) {
        if (fileManager->ExistDir(filePath) == false) {
            printf("[ERROR] Input isn't a directory!\n");
            HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[ERROR] Input file %s isn't a directory or not exist!", filePath.c_str());
            return false;
        }
    } else {
        printf("[ERROR] Input file type-- %d is invalid! 0: File, 1: Directory.\n", dirType);
        HIAI_ENGINE_LOG("[ERROR] Input file type-- %d is invalid! 0: File, 1: Directory.\n", dirType);
        return false;
    }

    return true;
}

bool ParseAndCheckArgs(int argc, char *argv[], DecodeInfo& decodeInfo)
{
    CommandParser options;
    options.addOption(HELP_CMD).addOption(I_CMD, DEFAULT_INPUT).addOption(R_CMD, "0").addOption(D_CMD, "-1");

    options.parseArgs(argc, argv);

    bool help = options.cmdOptionExists(HELP_CMD);
    decodeInfo.filePath = options.cmdGetOption(I_CMD);
    decodeInfo.dirType = parseStrToInt(options.cmdGetOption(R_CMD));
    decodeInfo.decodeType = parseStrToInt(options.cmdGetOption(D_CMD));

    if (!CheckArgs(help, decodeInfo.filePath, decodeInfo.decodeType, decodeInfo.dirType)) {
        return false;
    }
    return true;
}

HIAI_StatusT SendOneImagInfo(const string filePath, const string imageType, const uint32_t graphId,
                             const uint32_t engineId, shared_ptr<GraphManager> graphManager)
{
    char c[PATH_MAX + 1] = {0x00};
    errno_t err = strcpy_s(c, PATH_MAX + 1, filePath.c_str());
    if (err != EOK) {
        printf("[SrcEngine] strcpy %s failed!\n", c);
        return HIAI_ERROR;
    }
    char path[PATH_MAX + 1] = {0x00};
    char* pathRet = realpath(c, path);
    if (pathRet == NULL) {
        printf("File not exist %s...\n", path);
        return HIAI_ERROR;
    }
    printf("[main] Input file %s...\n", path);
    std::shared_ptr<ImageInputInfoT> imageInfo = make_shared<ImageInputInfoT>();
    imageInfo->imageType = imageType;
    imageInfo->filePath = filePath;

    HIAI_StatusT ret = graphManager->SendData(graphId, engineId, "ImageInputInfoT",
                                              std::static_pointer_cast<ImageInputInfoT>(imageInfo));
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Fail to send data! ret = %d", ret);
        return ret;
    }
    return HIAI_OK;
}

HIAI_StatusT SendBatchImageInfo(const string dirPath, const string imageType, const uint32_t graphId,
                                const uint32_t engineId, shared_ptr<GraphManager> graphManager)
{
    vector<string> imageTypeVector;
    shared_ptr<FileManager> fileManager(new FileManager);
    if (imageType == "") {
        imageTypeVector.push_back(IMAGE_TYPE[TYPE_JPEG]);
        imageTypeVector.push_back(IMAGE_TYPE[TYPE_PNG]);
    } else {
        imageTypeVector.push_back(imageType);
        //when image type is jpeg, add type of jpg
        if (imageType == IMAGE_TYPE[TYPE_JPEG]) {
            imageTypeVector.push_back(EXTENSION_JPG);
        }
    }

    vector<string> filesPath = fileManager->ReadByExtension(dirPath, imageTypeVector);
    for (int i = 0; i < filesPath.size(); i++) {
		// Sleep for a while to prevent pictures from being sent too fast and causing data congestion later.
		// Sleep time can be adjusted according to the time consumption of later business flows.
		usleep(1000);
        HIAI_StatusT ret = SendOneImagInfo(filesPath[i], imageType, graphId, engineId, graphManager);
        if (ret != HIAI_OK) {
            HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Fail to start graph. ret = %d", ret);
            return ret;
        }
    }

    return HIAI_OK;
}

int GetImageType(const string extension, const int32_t dvppType, string& imageType)
{
    string type = "";
    if (dvppType != DEFAULT_IMAGE_TYPE) {
        type = IMAGE_TYPE[dvppType];
        if (((extension == EXTENSION_JPEG || extension == EXTENSION_JPG)) && (dvppType != TYPE_JPEG)) {
            return -1;
        }

        if ((extension == EXTENSION_PNG) && (dvppType != TYPE_PNG)) {
            return -2;
        }
    } else {
        if (extension == EXTENSION_JPEG || extension == EXTENSION_JPG) {
            type = IMAGE_TYPE[TYPE_JPEG];
        } else if (extension == EXTENSION_PNG) {
            type = IMAGE_TYPE[TYPE_PNG];
        } else {
            // do nothing
        }
    }
    imageType = type;
    return 0;
}

HIAI_StatusT SendImageInfo(DecodeInfo& decodeInfo, const uint32_t graphId,
                           const uint32_t engineId, shared_ptr<GraphManager> graphManager)
{
    shared_ptr<FileManager> fileManager(new FileManager());
    string fileExtension = fileManager->GetExtension(decodeInfo.filePath);
    string imageType = "";
    // return "" means unknown format
    int res = GetImageType(fileExtension, decodeInfo.decodeType, imageType);
    if (res != 0) {
        printf("[ERROR]The input format is inconsistent with the decoding format. Please check!\n");
        return HIAI_ERROR;
    }

    HIAI_StatusT ret;
    if (!decodeInfo.dirType) {
        ret = SendOneImagInfo(decodeInfo.filePath, imageType, graphId, engineId, graphManager);
    } else {
        ret = SendBatchImageInfo(decodeInfo.filePath, imageType, graphId, engineId, graphManager);
    }
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Fail to get send data. ret = %d", ret);
        return HIAI_ERROR;
    }

    return HIAI_OK;
}

int main(int argc, char *argv[])
{
    // change to executable file directory
    shared_ptr<FileManager> fileManager(new FileManager());
    string path(argv[0], argv[0] + strlen(argv[0]));
    fileManager->ChangeDir(path.c_str());

    // Parsing and validation of input args
    DecodeInfo decodeInfo;
    if (!ParseAndCheckArgs(argc, argv, decodeInfo)) {
        return 0;
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
    // 3.send data
    ret = SendImageInfo(decodeInfo, GRAPH_ID, SRC_ENGINE_ID, graphManager);
    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Fail to send image info. ret = %d", ret);
        printf("[ERROR] Send Image info failed.\n");
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
    }
    printf("The sample end!\n");
    return 0;
}
