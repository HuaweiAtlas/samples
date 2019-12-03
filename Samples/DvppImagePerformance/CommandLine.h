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

#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include "FileManager/FileManager.h"
#include <algorithm>
#include <vector>

static HIAI_StatusT CheckEncodeArgs(const int width, const int height, const int format, const int level)
{
    int minWidth = 32;
    int maxWidth = 8192;
    if (width < minWidth || width > maxWidth || height < minWidth || height > maxWidth) {
        printf("[ERROR] the width/height is not valide , please check the help message\n");
        return HIAI_ERROR;
    }
    int minLevel = 1;
    int maxLevel = 100;
    if (level < minLevel || level > maxLevel) {
        printf("[ERROR] the level is not valide , please check the help message\n");
        return HIAI_ERROR;
    }
    std::vector<int> yuvType = { 0, 1, 2, 3, 16, 17 };
    if (find(yuvType.begin(), yuvType.end(), format) == yuvType.end()) {
        return HIAI_ERROR;
    }
    return HIAI_OK;
}

static HIAI_StatusT CheckArgs(const std::string& fileName, const int chipStartNo, const int chipEndNo, 
    const int graphPerChip, const int threadPerGraph, const int decodeType, const int width, 
    const int height, const int format, const int level)
{
    // check if the file exist
    shared_ptr<FileManager> fileManager(new FileManager());
    if (fileManager->ExistFile(fileName) == false) {
        printf("[%s] It is a directory, Please input file path\n", fileName.c_str());
        return HIAI_ERROR;
    }
    int minChipNo = 0;
    int maxChipNo = 3;
    if (chipStartNo < minChipNo || chipEndNo < minChipNo || chipStartNo > maxChipNo || chipEndNo > maxChipNo || chipEndNo < chipStartNo) {
        printf("[ERROR] the No of start or end of chips is not valid! Please check the help message\n");
        return HIAI_ERROR;
    }
    int minGraphNo = 1;
    int maxGraphNo = 10;
    if (graphPerChip < minGraphNo || graphPerChip > maxGraphNo) {
        printf("[ERROR] the numbers of graph per chip is not vaild, please check the help message\n");
        return HIAI_ERROR;
    }
    int minThreadNo = 1;
    int maxThreadNo = 16;
    if (threadPerGraph < minThreadNo || threadPerGraph > maxThreadNo) {
        printf("[ERROR] the number of thread per graph is not valid, please check the help message\n");
        return HIAI_ERROR;
    }
    int minDecodeType = 0;
    int maxDecodeType = 2;
    if (decodeType < minDecodeType || decodeType > maxDecodeType) {
        printf("[ERROR] the decodeType is not valide , please check the help message\n");
        return HIAI_ERROR;
    }
    if (decodeType == maxDecodeType) {
        return CheckEncodeArgs(width, height, format, level);
    }
    return HIAI_OK;
}

void showUsage() 
{
    std::cout << std::endl << "Usage: dvppTest [Options...]" << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "    -h                             " << "show usage message." << std::endl;
    std::cout << "    -d                             " << "dvpp type:" << std::endl;
    std::cout << "                                   " << "    0  - decode jpeg" << std::endl;
    std::cout << "                                   " << "    1  - decode png" << std::endl;
    std::cout << "                                   " << "    2  - encode jpeg" << std::endl;
    std::cout << "    -i                             " << "input file path" << std::endl;
    std::cout << "    -s                             " << "start chip id, default is 0. range[0, 3]" << std::endl;
    std::cout << "    -e                             " << "end chip id, default is 0. range[0, 3]" << std::endl;
    std::cout << "    -g                             " << "graph count per chip, default is 1. range[1, 10]" << std::endl;
    std::cout << "    -t                             " << "thread count per graph, default is 1. range[1, 16]" << std::endl;

    std::cout << "    -width                         " << "yuv pic width, default is 1. range[32, 8192], only encode jpeg need" << std::endl;
    std::cout << "    -height                        " << "yuv pic height, default is 1. range[32, 8192], only encode jpeg need" << std::endl;
    std::cout << "    -format                        " << "yuv pic format:" << std::endl;
    std::cout << "                                   " << "    0  - JPGENC_FORMAT_UYVY" << std::endl;
    std::cout << "                                   " << "    1  - JPGENC_FORMAT_VYUY" << std::endl;
    std::cout << "                                   " << "    2  - JPGENC_FORMAT_YVYU" << std::endl;
    std::cout << "                                   " << "    3  - JPGENC_FORMAT_YUYV" << std::endl;
    std::cout << "                                   " << "    16 - JPGENC_FORMAT_NV12" << std::endl;
    std::cout << "                                   " << "    17 - JPGENC_FORMAT_NV21" << std::endl;
    std::cout << "    -level                         " << "yuv pic level, default is 1. range[1, 100], only encode jpeg need" << std::endl << std::endl;

    std::cout << "Eg:" << std::endl;
    std::cout << "    ./main -d 0 -i ../data/test_1080p.jpg -s 0 -e 1 -g 1 -t 8" << std::endl << std::endl;
}

#endif
