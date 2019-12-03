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
#include <stdio.h>
#include <unistd.h>
#include "../Common/CommandParser.h"

using namespace std;

/// @brief Define flag for showing help message <br>
static const string HELP_MESSAGE = "Print a usage message.";
extern bool g_hExist;

const string I_MESSAGE = "Optional. Specify the input file: jpg iamge or H264 Video, default: test.jpeg";
extern string g_inputFile;

const string G_MESSAGE = "Optional. Specify the graph config file, default: graph.config";
extern string g_graphFile;

const string M_MESSAGE = "Optional. Specify the model file, default: load from graph.config";
extern string g_modelFile;

static const string C_MESSAGE =
    "Optional. Specify the number of video channle in one graph, range [1, 4], default: 1";
extern int g_videoChPerGraph;

const string S_MESSAGE = "start chip id, default is 0";
extern int g_deviceStart;

const string E_MESSAGE = "end chip id, default is 0";
extern int g_deviceEnd;

static bool ValidateInput(string flagName, const string &value)
{
    /* judge whether folder is existing: !(-1) -- exist */
    if ((access(value.c_str(), F_OK)) == -1) {
        printf("Parameter %s valid value: %s, file or folder does not exist!\n", flagName.c_str(), value.c_str());
        return false;
    } else {
        return true;
    }
}

const int MIN_CH_NUM_PER_GRAPH = 1;
const int MAX_CH_NUM_PER_GRAPH = 4;
static bool ValidateVideoChNumPerGraph(string flagName, const int value)
{
    /*
    * hardware limit: 
    *    h26* decode support 16 channel video maximum
    *    Infer model performance 
    * software limti:
    *    Create DecodeEngine Object Number in graph.config
 */
    if ((value < MIN_CH_NUM_PER_GRAPH) || (value > MAX_CH_NUM_PER_GRAPH)) {
        printf("Parameter %s valid value: %d, beyond [%d, %d]\n", flagName.c_str(), value, MIN_CH_NUM_PER_GRAPH,
               MAX_CH_NUM_PER_GRAPH);
        return false;
    } else {
        return true;
    }
}

const int MIN_DEVICE_ID = 0;
const int MAX_DEVICE_ID = 63;
static bool ValidateDeviceId(string flagName, const int value)
{
    /* Atlas driver request device id must be in [0, 63] */
    if ((value < MIN_DEVICE_ID) || (value > MAX_DEVICE_ID)) {
        printf("Parameter %s valid value: %d, beyond [%d, %d]\n", flagName.c_str(), value,
               MIN_DEVICE_ID, MAX_DEVICE_ID);
        return false;
    } else {
        return true;
    }
}
/**
* @brief This function show a help message
 */
static void ShowUsage(void)
{
    printf("\n");
    printf("ObjectDetection [OPTION]\n");
    printf("Options:\n");
    printf("\n");
    printf("    -h                             %s\n", HELP_MESSAGE.c_str());
    printf("    -i '<path>'                    %s\n", I_MESSAGE.c_str());
    printf("    -g '<graph file>'              %s\n", G_MESSAGE.c_str());
    printf("    -m '<model file>'              %s\n", M_MESSAGE.c_str());
    printf("    -c '<chanel number in graph>'  %s\n", C_MESSAGE.c_str());
    printf("    -s '<device id>'               %s\n", S_MESSAGE.c_str());
    printf("    -e '<device id>'               %s\n", E_MESSAGE.c_str());
    printf("\n");
    printf("Eg:");
    printf(" ./ObjectDetection -i ./test.jpg -m ./ObjectDetection.om -g ./graph.config -s 0 -e 1\n\n");
}

static bool ParseAndCheckCommandLine(int argc, char *argv[])
{
    // ---------------------------Parsing and validation of input args--------------------------------------
    CommandParser options;
    options.addOption("-h").addOption("-i", "test.jpeg").addOption("-g",
            "graph.config").addOption("-m").addOption("-c", "1").addOption("-s",
                    "0").addOption("-e", "0");

    options.parseArgs(argc, argv);
    g_hExist = options.cmdOptionExists("-h");
    g_inputFile = options.cmdGetOption("-i");
    g_graphFile = options.cmdGetOption("-g");
    bool modelExit = true;
    if (options.cmdOptionExists("-m")) {
        g_modelFile = options.cmdGetOption("-m");
        modelExit = ValidateInput("-m", g_modelFile) ? true : false;
    } else {
        g_modelFile = "";
    }
    g_deviceStart = parseStrToInt(options.cmdGetOption("-s"));
    g_deviceEnd = parseStrToInt(options.cmdGetOption("-e"));
    g_videoChPerGraph = parseStrToInt(options.cmdGetOption("-c"));

    if (g_hExist || (ValidateInput("-i", g_inputFile) == false) ||
        (ValidateInput("-g", g_graphFile) == false) ||
        (modelExit == false) || (ValidateDeviceId("-s", g_deviceStart) == false) ||
        (ValidateDeviceId("-e", g_deviceEnd) == false) ||
        (ValidateVideoChNumPerGraph("-c", g_videoChPerGraph) == false)) {
        ShowUsage();
        return false;
    }

    return true;
}

#endif  // COMMAND_LINE_H_
