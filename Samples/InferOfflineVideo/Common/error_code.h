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

#ifndef ERROR_CODE_H_
#define ERROR_CODE_H_

#include "hiaiengine/status.h"
#include "hiaiengine/log.h"

#define HIAI_INVALID_INPUT_MSG 001
#define HIAI_AI_MODEL_MANAGER_INIT_FAIL 002
#define HIAI_AI_MODEL_MANAGER_PROCESS_FAIL 003
#define HIAI_SEND_DATA_FAIL 004
#define HIAI_AI_MODEL_CREATE_OUTPUT_FAIL 005
#define HIAI_AI_MODEL_WRONG_OUTPUT_SIZE 006

/**
define error code for HIAI_ENGINE_LOG
**/
#define USE_DEFINE_ERROR 0x6001

enum {
    HIAI_IDE_ERROR_CODE,
    HIAI_IDE_INFO_CODE,
    HIAI_IDE_WARNING_CODE
};

HIAI_DEF_ERROR_CODE(USE_DEFINE_ERROR, HIAI_ERROR, HIAI_IDE_ERROR,
    "");
HIAI_DEF_ERROR_CODE(USE_DEFINE_ERROR, HIAI_INFO, HIAI_IDE_INFO,
    "");
HIAI_DEF_ERROR_CODE(USE_DEFINE_ERROR, HIAI_WARNING, HIAI_IDE_WARNING,
    "");

#define CHECK_RETURN_IF(x)                                                                                  \
    do {                                                                                                 \
        if (int(x)) {                                                                                    \
            HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Check failed %s %s line %d", __FILE__, __func__, __LINE__); \
            return HIAI_ERROR;                                                                           \
        }                                                                                                \
    } while (0)

/*
HIAI_DEF_ERROR_CODE(MODID_ENGINE, HIAI_ERROR, HIAI_INVALID_INPUT_MSG, \
    "invalid input message pointer");
HIAI_DEF_ERROR_CODE(MODID_ENGINE, HIAI_ERROR, HIAI_AI_MODEL_MANAGER_INIT_FAIL, \
    "ai model manager init failed");
HIAI_DEF_ERROR_CODE(MODID_ENGINE, HIAI_ERROR, HIAI_AI_MODEL_MANAGER_PROCESS_FAIL, \
    "ai model manager process failed");
HIAI_DEF_ERROR_CODE(MODID_ENGINE, HIAI_ERROR, HIAI_SEND_DATA_FAIL, \
    "send data failed");
HIAI_DEF_ERROR_CODE(MODID_ENGINE, HIAI_ERROR, HIAI_AI_MODEL_CREATE_OUTPUT_FAIL,
    "Failed to create output tensor");
HIAI_DEF_ERROR_CODE(MODID_ENGINE, HIAI_ERROR, HIAI_AI_MODEL_WRONG_OUTPUT_SIZE,
    "Faster rcnn output size is wrong");
*/

#endif // ERROR_CODE_H_
