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
#include <fstream>
#include <iostream>
#include <string>

static const int MIN_THREAD = 1;
static const int MAX_THREAD = 16;
static const int MIN_CHIPID = 0;
static const int MAX_CHIPID = 3;
static const int MIN_SIZE = 128;
static const int MAX_SIZE = 1920;
static const int MAX_GRAPH = 3;
static const int MAX_DECODE_TYPE = 3;
static const char HELP_MESSAGE[] = "Print a usage message.";
static const char I_MESSAGE[] = "Optional. Specify the input vedio, default: test.h264";
static const char CHIPSTART_MESSAGE[] = "Optional. Specify the start No of test chip, default: 0, range = [0, 3]";
static const char CHIPEND_MESSAGE[] = "Optional. Specify the end No of test chip, default: 0, range = [0, 3]";
static const char GRAPHPERCHIP_MESSAGE[] = "optional. Specify the numbers of graph per chip, default: 1, \
    need to less than 3 when encoding video";
static const char THREADPERGRAPH_MESSAGE[] = "optional. Specify the numbers of thread per graph, default: 1, \
    range = [1, 16] when -v = 0, range = [1] when -v = 1";
static const char DECODETYPE_MESSAGE[] = "optional. Specify the type of vedio file when decodingvideo, \
    0: h264, 1: h265, default: 0";
static const char ENCODETYPE_MESSAGE[] = "Specify the type of video after encoding yuv images, \
    0: h265 main level, 1: h264 baseline level, 2: h264 main level, 3: h264 high level, default: 0";
static const char DECORENC_MESSAGE[] = "Optional. Specify the test type, 0: decode video, 1: encode video, default: 0";
static const char HEIGHT_MESSAGE[] = "Optional. Specify the height of yuv image when encoding video, default: 1080 \
    range = [128, 1920]";
static const char WIDTH_MESSAGE[] = "Optional. Specify the width of yuv image when encoding video, default: 1920 \
    range = [128, 1920]";

static HIAI_StatusT CheckDecodeArgs(const int graphPerChip, const int threadPerGraph, const int decodeType)
{
    if (threadPerGraph < MIN_THREAD || threadPerGraph > MAX_THREAD) {
        printf("[ERROR] the number of thread per graph is not valid or null, please check the help message\n");
        return HIAI_ERROR;
    }
    if (decodeType < 0 || decodeType > 1) {
        printf("[ERROR] the decodeType is not valid , please check the help message\n");
        return HIAI_ERROR;
    }
    if (graphPerChip < 1) {
        printf("[ERROR] the numbers of graph per chip is less than 1, please check the help message\n");
        return HIAI_ERROR;
    }
    return HIAI_OK;
}

static HIAI_StatusT CheckEncodeArgs(const int graphPerChip, const int threadPerGraph, const int decodeType, 
    const int height, const int width)
{
    if (graphPerChip < 0 || graphPerChip > MAX_GRAPH) {
        printf("[ERROR] the numbers of graph per chip is not valid, please check the help message\n");
        return HIAI_ERROR;
    }
    if (threadPerGraph != MIN_THREAD) {
        printf("[ERROR] when encoding video, only support 1 thread per graph, please check the help message\n");
        return HIAI_ERROR;
    }
    if (decodeType < 0 || decodeType > MAX_DECODE_TYPE) {
        printf("[ERROR] the encodeType is not valid , please check the help message\n");
        return HIAI_ERROR;
    }
    if (height < MIN_SIZE || height > MAX_SIZE) {
        printf("[ERROR] the height is not valid, please check the hlep message\n");
        return HIAI_ERROR;
    }
    if (width < MIN_SIZE || width > MAX_SIZE) {
        printf("[ERROR] the width is not valid, please check the help message\n");
        return HIAI_ERROR;
    }
    return HIAI_OK;
}

static HIAI_StatusT CheckArgs(const std::string& fileName, const int chipStartNo, const int chipEndNo, 
    const int graphPerChip, const int threadPerGraph, const int decodeType, const int deOrEnc, 
    const int height, const int width)
{
    std::fstream fileTest;
    // check if the file exist
    fileTest.open(fileName.c_str(), std::ios::in);
    if (!fileTest) {
        printf("[ERROR] Input file %s doesn't exist!\n", fileName.c_str());
        return HIAI_ERROR;
    }
    fileTest.close();
    if (chipStartNo < MIN_CHIPID || chipEndNo < MIN_CHIPID || chipStartNo > MAX_CHIPID || chipEndNo > MAX_CHIPID || chipEndNo < chipStartNo) {
        printf("[ERROR] the No of start or end of chips is not valid! Please check the help message\n");
        return HIAI_ERROR;
    }
    if (deOrEnc == 0) {
        HIAI_StatusT ret = CheckDecodeArgs(graphPerChip, threadPerGraph, decodeType);
        if (ret != HIAI_OK) {
            return HIAI_ERROR;
        }
    } else if (deOrEnc == 1) {
        HIAI_StatusT ret = CheckEncodeArgs(graphPerChip, threadPerGraph, decodeType, height, width);
        if (ret != HIAI_OK) {
            return HIAI_ERROR;
        }
    } else {
        printf("[ERROR] the option of decode or encode video is not valid\n");
        return HIAI_ERROR;
    }
    return HIAI_OK;
}

/**
 * @brief This function show a help message
 */
static void ShowUsage()
{
    printf("\nUsage: main [Options...]\n\n");
    printf("Options:\n");
    printf("    -h                             %s\n", HELP_MESSAGE);
    printf("    -s                             %s\n", CHIPSTART_MESSAGE);
    printf("    -e                             %s\n", CHIPEND_MESSAGE);
    printf("    -g                             %s\n", GRAPHPERCHIP_MESSAGE);
    printf("    -t                             %s\n", THREADPERGRAPH_MESSAGE);
    printf("    -i '<path>'                    %s\n", I_MESSAGE);
    printf("    -d                             %s\n", DECODETYPE_MESSAGE);
    printf("                                   %s\n", ENCODETYPE_MESSAGE);
    printf("    -v                             %s\n", DECORENC_MESSAGE);
    printf("    -height                        %s\n", HEIGHT_MESSAGE);
    printf("    -width                         %s\n", WIDTH_MESSAGE);
}

#endif
