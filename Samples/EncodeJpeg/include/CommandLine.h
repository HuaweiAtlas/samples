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

static const int YUV422_YUYV = 3;
static const int YUV420_NV12 = 16;
static const int YUV420_NV21 = 17;
static const char HELP_MESSAGE[] = "Print a usage message.";
static const char I_MESSAGE[] = "Optional. Specify the input image, default: ../data/test.yuv";
static const char HEIGHT_MESSAGE[] = "Requested. Specify the height of input image";
static const char WIDTH_MESSAGE[] = "Requested. Specify the width of input image";
static const char FORMAT_MESSAGE[] = "Requested. Specify the format of input image, \
    YUV422: {UYVY:0, VYUY:1, YVYU:2, YUYV:3}, YUV420: {NV12:16, NV21:17}";

/*
 * function: check the validity of input argument
 * params：fileName, name of input file
 * params: height, height of input image
 * params: width, width of input image
 * params: format, format of input image
 * return: HIAI_StatusT, the validty
 */
static HIAI_StatusT CheckArgs(const std::string& fileName, const int height, const int width, const int format)
{
    std::fstream fileTest;
    // check if the file exist
    fileTest.open(fileName.c_str(), std::ios::in);
    if (!fileTest) {
        printf("[ERROR] Input file %s doesn't exist!\n", fileName.c_str());
        return HIAI_ERROR;
    }
    fileTest.close();
    if (height <= 0 || width <= 0) {
        printf("[ERROR] the height or width is not valid or given, please check the help message\n");
        return HIAI_ERROR;
    }
    if (format < 0 || format > YUV420_NV21) {
        printf("[ERROR] the format is not valid or given, please check the help message\n");
        return HIAI_ERROR;
    } else if (format > YUV422_YUYV && format < YUV420_NV12) { 
        printf("[ERROR] the format is not valid or given, please check the help message\n");
        return HIAI_ERROR;
    }
    return HIAI_OK;
}

/**
 * @brief This function show a help message
 */
static void ShowUsage()
{
    printf("\nUsage: dvppTest [Options...]\n\n");
    printf("Options:\n");
    printf("    -h                             %s\n", HELP_MESSAGE);
    printf("    -height                        %s\n", HEIGHT_MESSAGE);
    printf("    -width                         %s\n", WIDTH_MESSAGE);
    printf("    -format                        %s\n", FORMAT_MESSAGE);
    printf("    -i '<path>'                    %s\n", I_MESSAGE);
}

#endif
