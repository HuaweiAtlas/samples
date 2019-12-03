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

#ifndef COMMAND_LINE_H_
#define COMMAND_LINE_H_
#include <string>
using namespace std;

struct DecodeInfo{
    std::string filePath;
    int isDir;
    int decodeType;
};

/// @brief Define flag for showing help message <br>
static const char HELP_MESSAGE[] = "Print a usage message.";
static const char R_MESSAGE[] = "Optional. Input path type, default: 0";
static const char I_MESSAGE[] = "Optional. Specify the image, default: ../data/test.jpeg.";
static const char D_MESSAGE[] = "Optional. Specify the dvpp type. default: -1";

static const string HELP_CMD = "-h";
static const string R_CMD = "-r";
static const string I_CMD = "-i";
static const string D_CMD = "-d";

/**
* @brief This function show a help message
 */
static void ShowUsage()
{
    printf("\n");
    printf("Options:\n");
    printf("\n");
    printf("    -h                         %s\n", HELP_MESSAGE);
    printf("    -i                         %s\n", I_MESSAGE);
    printf("    -r                         %s\n", R_MESSAGE);
    printf("                                   0 - Input path is a file.\n");
    printf("                                   1 - Input path is a directory.\n");
    printf("    -d                         %s\n", D_MESSAGE);
    printf("                                   0 - decode jpeg\n");
    printf("                                   1 - decode png \n");
    printf("                               Jpeg resolutions: maximum 8192 x 8192, minimum 32 x 32.\n");
    printf("                               Png  resolutions: maximum 4096 x 4096, minimum 32 x 32.\n");
}

#endif  // COMMAND_LINE_H_
