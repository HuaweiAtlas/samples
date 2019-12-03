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
#include "main.h"
#include "hiaiengine/api.h"
#include "hiaiengine/ai_memory.h"
#include "AppCommon.h"
#include "DataRegister.h"
#include "CommandLine.h"
#include "CommandParser.h"
#include "DynamicGraph.h"

using namespace std;
using namespace hiai;
using namespace google;

int g_graphRecived;

HIAI_StatusT CustomDataRecvInterface::RecvData(const shared_ptr<void> &message)
{
    g_graphRecived--;
    return HIAI_OK;
}

int CommandRead(int argc, char *argv[], shared_ptr<CommandParaT> cmdPara)
{
    CommandParser options;
    options.addOption("-h")
    .addOption("-i", "")
    .addOption("-s", "0")
    .addOption("-e", "0")
    .addOption("-g", "1")
    .addOption("-t", "1")
    .addOption("-d", "0")
    .addOption("-width", "1")
    .addOption("-height", "1")
    .addOption("-format", "1")
    .addOption("-level", "1");
    options.parseArgs(argc, argv);
    cmdPara->help = options.cmdOptionExists("-h");
    cmdPara->fileName = options.cmdGetOption("-i");
    cmdPara->chipStartNo = parseStrToInt(options.cmdGetOption("-s"));
    cmdPara->chipEndNo = parseStrToInt(options.cmdGetOption("-e"));
    cmdPara->graphPerChip = parseStrToInt(options.cmdGetOption("-g"));
    cmdPara->threadPerGraph = parseStrToInt(options.cmdGetOption("-t"));
    cmdPara->decodeType = parseStrToInt(options.cmdGetOption("-d"));
    cmdPara->width = parseStrToInt(options.cmdGetOption("-width"));
    cmdPara->height = parseStrToInt(options.cmdGetOption("-height"));
    cmdPara->format = parseStrToInt(options.cmdGetOption("-format"));
    cmdPara->level = parseStrToInt(options.cmdGetOption("-level"));
    // check the validity of input argument
    if (cmdPara->help ||
        HIAI_OK != CheckArgs(cmdPara->fileName, cmdPara->chipStartNo, cmdPara->chipEndNo, cmdPara->graphPerChip,
                             cmdPara->threadPerGraph, cmdPara->decodeType, cmdPara->width, cmdPara->height, 
                             cmdPara->format, cmdPara->level)) {
        showUsage();
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    HIAI_StatusT ret = HIAI_OK;
    shared_ptr<CommandParaT> cmdPara = make_shared<CommandParaT>();
    if (CommandRead(argc, argv, cmdPara) != 0) {
        return -1;
    }
    g_graphRecived = (cmdPara->chipEndNo - cmdPara->chipStartNo + 1) * cmdPara->graphPerChip;
    shared_ptr<DynamicGraph> dynamicGraph(new DynamicGraph(cmdPara->chipStartNo, cmdPara->chipEndNo,
                                                           cmdPara->graphPerChip, cmdPara->threadPerGraph, (TestType)(cmdPara->decodeType)));
    if (HIAI_OK != dynamicGraph->InitAndStartGraph()) {
        cout << "Fail to start graph " << ret << endl;
        return -1;
    }
    // read test file
    char c[PATH_MAX + 1] = { 0x00 };
    errno_t err = strcpy_s(c, PATH_MAX + 1, cmdPara->fileName.c_str());
    if (err != EOK) {
        printf("[ERROR] strcpy %s failed!\n", c);
        return -1;
    }

    char path[PATH_MAX + 1] = { 0x00 };
    if ((strlen(c) > PATH_MAX) || (realpath(c, path) == NULL)) {
        printf("Get canonnicalize path fail!\n");
        return -1;
    }
    FILE *fp = fopen(path, "rb");
    if (fp == nullptr) {
        printf("Open file failed!");
        return HIAI_ERROR;
    }
    fseek(fp, 0, SEEK_END);
    int fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unsigned char *buffer = nullptr;
    int dMallocDelay = 10000;
    ret = HIAIMemory::HIAI_DMalloc(fileSize, (void *&)buffer, dMallocDelay);
    if (ret != HIAI_OK || buffer == nullptr) {
        HIAI_ENGINE_LOG("HIAI_DMalloc fail!ret = %d", ret);
        fclose(fp);
        return -1;
    }
    if (fileSize > 0) {
        int readLen = fread(buffer, 1, fileSize, fp);
    }
    fclose(fp);
    shared_ptr<CustomDataRecvInterface> recvInterface(new CustomDataRecvInterface());
    dynamicGraph->SetAllGraphRecvFunctor(recvInterface);
    dynamicGraph->SendAllGraphData(buffer, fileSize, cmdPara->width, cmdPara->height, cmdPara->format, cmdPara->level);
    int sleepTime = 100000;
    while (g_graphRecived > 0) {
        usleep(sleepTime);
    }
    dynamicGraph->DestroyAllGraph();
    return 0;
}
