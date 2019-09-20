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
#include "app_common.h"
#include <fstream>
#include <iostream>
#include <string>

// max decode channels
static const uint32_t MAX_CHANNELS = 16;

static HIAI_StatusT checkArgs(const std::string& filename, const int decode, const int groups)
{
    std::fstream fileTest;
    fileTest.open(filename.c_str(), std::ios::in);
    if (!fileTest) {
        printf("[ERROR] Input file %s doesn't exit!\n", filename.c_str());
        return HIAI_ERROR;
    }
    fileTest.close();
    if (groups <= 0 || groups > MAX_CHANNELS) {
        printf("invalid input params g %d\n", groups);
        return HIAI_ERROR;
    }
    if (decode < 0 || decode > 1) {
        printf("invalid input params d %d\n", decode);
        return HIAI_ERROR;
    }
    return HIAI_OK;
}

static void showUsage()
{
    printf("Usage: main [Options...]\n");
    printf("Options:\n");
    printf("    -h                             show usage message.\n");
    printf("    -i                             input file path\n");
    printf("    -d                             dvpp type:\n");
    printf("                                   0 - decode h264\n");
    printf("                                   1 - decode h265\n");
    printf("    -g                             graph count per chip, default is 1\n");
    printf("Eg:\n");
    printf("    ./main -i /path/to/video/file.264\n");
}

#endif
