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
#include <sys/stat.h>
#include <dirent.h>
#include <thread>
#include <fstream>
#include <algorithm>
#include <libgen.h>
#include <string>
#include <map>
#include "proto/graph_config.pb.h"
#include "hiaiengine/api.h"

#include "CommandLine.h"
#include "common_data_type.h"
#include "CustomDataRecvInterface.h"

HIAI_REGISTER_SERIALIZE_FUNC("StreamRawData", StreamRawData, StreamRawDataSerialize, StreamRawDataDeserialize);

bool g_hExist = false;
std::string g_inputFile = "";
std::string g_graphFile = "";
std::string g_modelFile = "";
int g_videoChPerGraph = 0;
int g_deviceStart = 0;
int g_deviceEnd = 0;

int g_graphStart = 1;
bool g_signalReceived = false;
std::map<int, bool> g_graphFlag;
int g_jpgIndex = 0;

const int DELAY_1_S = 1000000;
const int DELAY_200_MS = 200000;
const int VIDEO_BUF_SIZE = 1048576;  // 1M
const int VDEC_ENGINE_ID_START = 1000;
const int DST_ENGINE_ID = 2002;


void SigalHandler(int signo)
{
    if (signo == SIGINT) {
        g_signalReceived = true;
    }
}

int ReadJpg(std::string &fileName, RawDataBuffer &jpegBuf)
{
    std::ifstream jpegFile;
    jpegFile.open(fileName.c_str(), std::ios::binary);  // open input file
    if (jpegFile.is_open() == false) {
        printf("[ERROR]: In ReadJpg, read %s error\n", fileName.c_str());
    }
    jpegFile.seekg(0, std::ios::end);  // go to the end
    int length = jpegFile.tellg();     // report location (this is the length)
    int bufferLen = length + 8;        // for dvpp jpeg decode, Jpeg + 8
    jpegFile.seekg(0, std::ios::beg);  // go back to the beginning

    jpegBuf.len_of_byte = bufferLen;
    uint8_t *allocBuffer = nullptr;
    HIAI_StatusT ret = hiai::HIAIMemory::HIAI_DMalloc(bufferLen, (void *&)allocBuffer);
    if (ret != HIAI_OK || allocBuffer == nullptr) {
        printf("[ERROR]: read %s, HIAI_DMalloc() error\n", fileName.c_str());
        return -1;
    }
    /* allocBuffer will be free in SendData */
    jpegBuf.data.reset(allocBuffer, [](uint8_t *p) {});

    jpegFile.read(reinterpret_cast<char *>(allocBuffer), length);  // read the whole file into the buffer
    jpegFile.close();                            // close file handle

    return 0;
}

void SendJpgEOSToGraph(void)
{
    std::shared_ptr<StreamRawData> streamData = std::make_shared<StreamRawData>();
    streamData->info.mode = 0;
    streamData->info.format = 0;
    streamData->info.isEOS = 1;

    for (int deviceId = g_deviceStart; deviceId < g_deviceEnd + 1; deviceId++) {
        int graphId = deviceId + g_graphStart;
        std::shared_ptr<hiai::Graph> graph = hiai::Graph::GetInstance(graphId);
        if (nullptr == graph) {
            printf("[Error] graph is null in SendEOSToGraph()\n");
            return;
        }
        for (int channel = 0; channel < g_videoChPerGraph; channel++) {
            streamData->info.channelId = channel;
            hiai::EnginePortID engineId;
            engineId.graph_id = graphId;
            engineId.port_id = 0;
            engineId.engine_id = VDEC_ENGINE_ID_START + channel;

            HIAI_StatusT hiaiRet = graph->SendData(engineId, "StreamRawData",
                                                   std::static_pointer_cast<void>(streamData));
            if (hiaiRet != HIAI_OK) {
                printf("call memcpy_s fail\n");
                return;
            }
        }
    }
}

int ImageProcess(std::string &fileName)
{
    std::shared_ptr<StreamRawData> streamData = std::make_shared<StreamRawData>();
    streamData->info.mode = MODE_NORMAL;
    streamData->info.format = FORMAT_JPG;
    streamData->info.isEOS = 0;
    streamData->info.frameId = g_jpgIndex++;

    for (int deviceId = g_deviceStart; deviceId < g_deviceEnd + 1; deviceId++) {
        int graphId = deviceId + g_graphStart;
        std::shared_ptr<hiai::Graph> graph = hiai::Graph::GetInstance(graphId);
        if (nullptr == graph) {
            return -1;
        }
        for (int channel = 0; channel < g_videoChPerGraph; channel++) {
            /*
            * In high speed data transmission mode, data buffer need to be applied by HIAI_DMalloc(), and be free automatically by framework;
            * The same data buffer cannot be sent twice, so apply memory separately for each channel
            */
            if (0 != ReadJpg(fileName, streamData->buf)) {
                printf("[ERROR]: read %s error\n", fileName.c_str());
                return -1;
            }

            streamData->info.channelId = channel;
            hiai::EnginePortID engineId;
            engineId.graph_id = graphId;
            engineId.port_id = 0;
            engineId.engine_id = VDEC_ENGINE_ID_START + channel;

            HIAI_StatusT hiaiRet = graph->SendData(engineId, "StreamRawData",
                                                   std::static_pointer_cast<void>(streamData));
            if (hiaiRet != HIAI_OK) {
                printf("call memcpy_s fail\n");
                return hiaiRet;
            }
        }
    }

    return 0;
}

int OfflineVideoProcess(std::string inputVideo)
{
    HIAI_StatusT ret = HIAI_OK;
    int cnt = 0;
    int isEOS = 0;

    std::ifstream videoFile;
    videoFile.open(inputVideo.c_str(), std::ios::binary | std::ios::in);  // open input file
    if (videoFile.is_open() == false) {
        printf("[ERROR]: In ReadJpg, read %s error\n", inputVideo.c_str());
        return -1;
    }
    char *videoBufferPtr = new char[VIDEO_BUF_SIZE];

    int readLen = VIDEO_BUF_SIZE;
    while ((!g_signalReceived) && (isEOS == 0)) {
        videoFile.read(static_cast<char *>(videoBufferPtr), VIDEO_BUF_SIZE);
        readLen = videoFile.gcount();
        if (videoFile.eof()) {
            videoFile.close();  // close file handle
            isEOS = 1;
        }

        std::shared_ptr<StreamRawData> streamData = std::make_shared<StreamRawData>();
        streamData->info.mode = MODE_NORMAL;
        streamData->info.format = FORMAT_H264;
        streamData->info.frameId = cnt++;
        streamData->info.isEOS = isEOS;

        for (int deviceId = g_deviceStart; deviceId < g_deviceEnd + 1; deviceId++) {
            int graphID = deviceId + g_graphStart;
            std::shared_ptr<hiai::Graph> graph = hiai::Graph::GetInstance(graphID);
            if (nullptr == graph) {
                videoFile.close();  // close file handle
                delete[] videoBufferPtr;
                return -1;
            }

            for (int channel = 0; channel < g_videoChPerGraph; channel++) {
                streamData->info.channelId = channel;
                /*
                 * In high speed data transmission mode, data buffer need to be applied by HIAI_DMalloc(), and be free automatically by framework;
                 * The same data buffer cannot be sent twice, so apply memory separately for each channel
                */
                streamData->buf.len_of_byte = readLen;
                uint8_t *allocBuffer = nullptr;
                ret = hiai::HIAIMemory::HIAI_DMalloc(readLen, (void *&)allocBuffer);
                if (ret != HIAI_OK) {
                    videoFile.close();  // close file handle
                    delete[] videoBufferPtr;
                    printf("[ERROR]: ch %d, size: %d, HIAI_DMalloc() error\n", streamData->info.channelId, readLen);
                    return -1;
                }
                /* allocBuffer will be free in SendData */
                streamData->buf.data.reset(allocBuffer, [](uint8_t *data) {});
                errno_t ret = memcpy_s(streamData->buf.data.get(), streamData->buf.len_of_byte, videoBufferPtr, readLen);
                if (ret != EOK) {
                    printf("[ERROR] memcpy_s of main() is wrong\n");
                    delete[] videoBufferPtr;
                    return HIAI_ERROR;
                }

                hiai::EnginePortID engineId;
                engineId.graph_id = graphID;
                engineId.port_id = 0;
                engineId.engine_id = VDEC_ENGINE_ID_START + channel;
                ret = graph->SendData(engineId, "StreamRawData", std::static_pointer_cast<void>(streamData));
                if (ret != HIAI_OK) {
                    videoFile.close();  // close file handle
                    delete[] videoBufferPtr;
                    printf("[ERROR] main Senddata error!\n");
                    return -1;
                }
            }
        }

        usleep(DELAY_200_MS);
    }

    videoFile.close();  // close file handle
    delete[] videoBufferPtr;

    return 0;
}

bool JudgeFileType(std::string &file, std::string type)
{
    if (type == file.substr(file.length() - type.length(), file.length())) {
        return true;
    } else {
        return false;
    }
}

int InputFileProcess(std::string &inputFile)
{
    if (JudgeFileType(inputFile, "264")) {
        OfflineVideoProcess(inputFile);
    } else if (JudgeFileType(inputFile, "jpg") || JudgeFileType(inputFile, "jpeg")) {
        ImageProcess(inputFile);
        /* send the EOS to Graph */
        SendJpgEOSToGraph();
    } else {
        printf("[ERROR] Don't process file: %s \n", inputFile.c_str());
        return -1;
    }

    return 0;
}

void InputFolderProcess(std::string &inputFolder)
{
    DIR *dir = opendir(inputFolder.c_str());  // return value for opendir()

    if (NULL == dir) {
        printf("[ERROR] Don't Open folder %s \n", inputFolder.c_str());
        return;
    }

    /* read all the files in the dir ~ */
    struct dirent *fileName = readdir(dir);  // return value for readdir()
    while (fileName != NULL) {
        // get rid of "." and ".."
        if (strcmp(fileName->d_name, ".") == 0 || strcmp(fileName->d_name, "..") == 0) {
            fileName = readdir(dir);
            continue;
        }

        std::string inputFile = inputFolder + "/" + std::string(fileName->d_name);
        if (JudgeFileType(inputFile, "jpg") || JudgeFileType(inputFile, "jpeg")) {
            ImageProcess(inputFile);
        }

        /* delay 200ms in each jpg file */
        usleep(DELAY_200_MS);
        fileName = readdir(dir);
    }
    closedir(dir);

    /* send the EOS to Graph */
    SendJpgEOSToGraph();
}

/**
* @ingroup CustomDataRecvInterface
* @brief RecvData get data from graph 
* @param [in]
 */
HIAI_StatusT CustomDataRecvInterface::RecvData(const std::shared_ptr<void> &message)
{
    std::shared_ptr<DeviceStreamData> inputArg = std::static_pointer_cast<DeviceStreamData>(message);

    SetChFlag(inputArg->info.channelId);

    bool flag = GetGraphFlag();

    if (flag) {
        std::map<int, bool>::iterator iter;
        iter = g_graphFlag.find(graphID);
        if (iter != g_graphFlag.end()) {
            iter->second = true;
            printf("[INFO] graphID:%d is over!\n", graphID);
        }
    }

    return HIAI_OK;
}

void ModifyModelInGraph(std::string &modelFile, hiai::GraphConfigList &graphConfigList)
{
    /* Get graph */
    hiai::GraphConfig *graphcfg = graphConfigList.mutable_graphs(0);
    for (int i = 0; i < graphcfg->engines_size(); i++) {
        hiai::EngineConfig *engCfgPtr = graphcfg->mutable_engines(i);

        /* Get engine: ObjectDetectionEngine */
        if (*(engCfgPtr->mutable_engine_name()) == "ObjectDetectionEngine") {
            if (engCfgPtr->has_ai_config() != true) {
                printf("[ERROR] ObjectDetectionEngine engine don't have ai_config!\n");
                break;
            }
            hiai::AIConfig *aiCfgPtr = engCfgPtr->mutable_ai_config();
            for (int j = 0; j < aiCfgPtr->items_size(); j++) {
                hiai::AIConfigItem *itemCfgPtr = aiCfgPtr->mutable_items(j);
                
                /* Get item: Model */
                if (*(itemCfgPtr->mutable_name()) == "model") {
                    itemCfgPtr->set_value(g_modelFile);
                }
            }
        }
    }
}

// Init and create graph
HIAI_StatusT HIAI_InitAndStartGraph(void)
{
    HIAI_StatusT status;
    hiai::GraphConfigList graphConfigList;

    status = hiai::Graph::ParseConfigFile(g_graphFile.c_str(), graphConfigList);
    if (status != HIAI_OK) {
        printf("ParseConfigFile [%s] error\n", "./graphYoloGlod.config");
        return status;
    }

    /* reset the model from command line parameter */
    if (!g_modelFile.empty()) {
        ModifyModelInGraph(g_modelFile, graphConfigList);
    }

    for (int id = g_deviceStart; id < g_deviceEnd + 1; id++) {
        int graphID = g_graphStart + id;
        status = HIAI_Init(id);
        hiai::GraphConfig *graphcfg = graphConfigList.mutable_graphs(0);
        graphcfg->set_device_id(std::to_string(id));
        graphcfg->set_graph_id(graphID);

        status = hiai::Graph::CreateGraph(graphConfigList);
        if (status != HIAI_OK) {
            printf("CreateGraph %d error\n", graphID);
            return status;
        }

        /*
        * register callback function: send data from graph(engine 2002, output port 0) to callback function: CustomDataRecvInterface::RecvData()
        */
        std::shared_ptr<hiai::Graph> graph = hiai::Graph::GetInstance(graphID);
        if (nullptr == graph) {
            printf("Fail to get the graph-%u", graphID);
            return status;
        }

        printf("device = %d, graphID = %d init success\n", id, graphID);

        hiai::EnginePortID targetPortConfig;
        targetPortConfig.graph_id = graphID;
        targetPortConfig.engine_id = DST_ENGINE_ID;
        targetPortConfig.port_id = 0;
        graph->SetDataRecvFunctor(targetPortConfig, std::shared_ptr<CustomDataRecvInterface>( \
                                                    new CustomDataRecvInterface(graphID, g_videoChPerGraph)));

        /* initial the flag of graph */
        g_graphFlag.insert(std::pair<int, bool>(graphID, false));
    }

    return HIAI_OK;
}

bool IsFolder(std::string &inputFile)
{
    struct stat s;
    if (stat(inputFile.c_str(), &s) == 0) {
        if (s.st_mode & S_IFDIR) {
            return true;
        }
    }

    return false;
}

int main(int argc, char *argv[])
{
    /* register signal of the "ctrl + c" */
    if (signal(SIGINT, SigalHandler) == SIG_ERR) {
        printf("\ncan't catch SIGINT\n");
    }

    // 0.parse the command line
    if (ParseAndCheckCommandLine(argc, argv) != true) {
        return 0;
    }
    printf("***start device %d, end device %d***\n", g_deviceStart, g_deviceEnd);

    // 1.create graph
    HIAI_StatusT ret = HIAI_InitAndStartGraph();
    if (ret != HIAI_OK) {
        return -1;
    }

    // 2.Main Process
    if (IsFolder(g_inputFile)) {
        InputFolderProcess(g_inputFile);
    } else {
        InputFileProcess(g_inputFile);
    }

    // 3.Exit
    while (g_signalReceived == false) {
        usleep(DELAY_1_S);
        bool exitFlag = true;
        std::map<int, bool>::iterator iter;
        for (iter = g_graphFlag.begin(); iter != g_graphFlag.end(); iter++) {
            exitFlag = iter->second && exitFlag;
        }
        if (exitFlag) {
            break;
        }
    }

    // 4.destroy graph
    g_graphFlag.clear();
    for (int id = g_deviceStart; id < g_deviceEnd + 1; id++) {
        hiai::Graph::DestroyGraph(g_graphStart + id);
    }

    return 0;
}
