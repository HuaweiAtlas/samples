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
#include <atomic>
#include <iostream>
#include <libgen.h>
#include <unistd.h>
#include "hiaiengine/api.h"
#include "hiaiengine/ai_memory.h"
#include "DataRecv.h"
#include "DynamicGraph.h"
#include "AppCommon.h"
#include "CommandParser.h"
#include "CommandLine.h"
#include "FileManager/FileManager.h"

int g_graphReceived = 0;
static const int TIME_OUT_MS = 10000;

HIAI_StatusT CustomDataRecvInterface::RecvData(const std::shared_ptr<void>& message)
{
    g_graphReceived--;
    return HIAI_OK;
}

HIAI_StatusT ReadFile(std::string fileName, unsigned char*& buffer, int& fileSize)
{
    shared_ptr<FileManager> fileManager(new FileManager());
    if (fileManager->ExistDir(fileName) == true) {
        printf("[ERROR] Input file %s isn't a file or not exist!\n", fileName.c_str());
        return HIAI_ERROR;
    }
    char c[PATH_MAX + 1] = {0x00};
    errno_t err = strcpy_s(c, PATH_MAX + 1, fileName.c_str());
    if (err != EOK) {
        printf("[ERROR] strcpy %s failed!\n", c);
        return HIAI_ERROR;
    }

    char path[PATH_MAX + 1] = {0x00};
    if (realpath(c, path) == NULL) {
        printf("file not exit %s\n", path);
        return HIAI_ERROR;
    }
    FILE *fp = fopen(path, "rb");
    if (fp == nullptr) {
        printf("Open file failed!");
        return HIAI_ERROR;
    }
    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    HIAI_StatusT ret = hiai::HIAIMemory::HIAI_DMalloc(fileSize, (void*&)buffer, TIME_OUT_MS);
    if (ret != HIAI_OK || buffer == nullptr) {
        HIAI_ENGINE_LOG("HIAI_DMalloc fail!ret = %d", ret);
        fclose(fp);
        return HIAI_ERROR;
    }
    if (fileSize > 0) {
        int readLen = fread(buffer, 1, fileSize, fp);
    }
    fclose(fp);
    return HIAI_OK;
}

int main(int argc, char* argv[])
{
    HIAI_StatusT ret;
    CommandParser options;
    options.addOption("-h").addOption("-i", "test.h264").addOption("-s", "0").addOption("-e", "0")
        .addOption("-g", "1").addOption("-t", "1").addOption("-d", "0").addOption("-v", "0")
        .addOption("-height", "1080").addOption("-width", "1920");
    options.parseArgs(argc, argv);
    bool help = options.cmdOptionExists("-h");
    std::string fileName = options.cmdGetOption("-i");
    int chipStartNo = parseStrToInt(options.cmdGetOption("-s"));
    int chipEndNo = parseStrToInt(options.cmdGetOption("-e"));
    int graphPerChip = parseStrToInt(options.cmdGetOption("-g"));
    int threadPerGraph = parseStrToInt(options.cmdGetOption("-t"));
    int decodeType = parseStrToInt(options.cmdGetOption("-d"));
    int deOrEnc = parseStrToInt(options.cmdGetOption("-v"));
    int height = parseStrToInt(options.cmdGetOption("-height"));
    int width = parseStrToInt(options.cmdGetOption("-width"));
    // check the validity of input argument
    if (help || HIAI_OK != CheckArgs(fileName, chipStartNo, chipEndNo, graphPerChip, threadPerGraph, decodeType, deOrEnc, height, width)) {
        ShowUsage();
        return -1;
    }
    g_graphReceived = (chipEndNo - chipStartNo + 1) * graphPerChip;
    std::shared_ptr<DynamicGraph> dynamicGraph(new DynamicGraph(chipStartNo, chipEndNo, 
        graphPerChip, threadPerGraph, decodeType, deOrEnc, height, width));
    if (HIAI_OK != dynamicGraph->InitAndStartGraph()) {
        printf("Failed to start graph\n");
        dynamicGraph->DestroyAllGraph();
        return -1;
    }
    std::shared_ptr<CustomDataRecvInterface> recvInterface(new CustomDataRecvInterface(""));
    ret = dynamicGraph->SetAllGraphRecvFunctor(recvInterface);
    if (ret != HIAI_OK) {
        dynamicGraph->DestroyAllGraph();
        return -1;
    }
    unsigned char* buffer = nullptr;
    int fileSize;
    ret = ReadFile(fileName, buffer, fileSize);
    if (ret != HIAI_OK) {
        dynamicGraph->DestroyAllGraph();
        return -1;
    }
    ret = dynamicGraph->SendAllGraphData(buffer, fileSize);
    if (ret != HIAI_OK) {
        dynamicGraph->DestroyAllGraph();
        return -1;
    }
    while (g_graphReceived > 0) {
        usleep(TIME_OUT_MS);
    }
    dynamicGraph->DestroyAllGraph();
    return 0;
}
