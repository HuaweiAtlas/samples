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

#ifndef ERROR_CODE_H
#define ERROR_CODE_H

#include "hiaiengine/status.h"
#include <string>

using namespace std;

/*
* define error code for HIAI_ENGINE_LOG
 */
#define MODID_APP_DEFINE 0x6001

const int ERROR_CODE_NUM = 26;

enum {
    APP_OK_CODE = 0,        // 成功
    APP_FAIL_CODE,          // 一般性错误
    APP_INNER_CODE,         // 内部错误（一般在同一个模块内使用，不对外公开
    APP_POINTER_CODE,       // 非法指针
    APP_INVALARG_CODE,      // 非法参数
    APP_NOTIMPL_CODE = 5,   // 功能未实现
    APP_OUTOFMEM_CODE,      // 内存申请失败/内存不足
    APP_BUFERROR_CODE,      // 缓冲区错误（不足，错乱）
    APP_PERM_CODE,          // 权限不足，访问未授权的对象
    APP_TIMEOUT_CODE,       // 超时
    APP_NOTINIT_CODE = 10,  // 未初始化
    APP_INITFAIL_CODE,      // 初始化失败
    APP_ALREADY_CODE,       // 已初始化，已经在运行
    APP_INPROGRESS_CODE,    // 已在运行、进行状态
    APP_EXIST_CODE,         // 申请资源对象(如文件或目录)已存在
    APP_NOTEXIST_CODE,      // 资源对象(如文件或目录)、命令、设备等不存在
    APP_BUSY_CODE,          // 设备或资源忙（资源不可用）
    APP_FULL_CODE,          // 设备/资源已满
    APP_EMPTY_CODE,         // 对象/内存/内容为空
    APP_OPENFAIL_CODE,      // 资源对象(如文件或目录、socket)打开失败
    APP_READFAIL_CODE,      // 资源对象(如文件或目录、socket)读取、接收失败
    APP_WRITEFAIL_CODE,     // 资源对象(如文件或目录、socket)写入、发送失败
    APP_DELFAIL_CODE,       // 资源对象(如文件或目录、socket)删除、关闭失败

    /* just for call HIAI_ENGINE_LOG() to output log */
    APP_INFO_CODE,
    APP_WARNING_CODE,
    APP_ERROR_CODE,
};

const string APP_LOG_STRING[ERROR_CODE_NUM] = {
    "Success",
    "General Failed",
    "Internal error",
    "Invalid Pointer",
    "Invalid argument",
    "Not implemented",
    "Out of memory",
    "Buffer error",
    "Permission denied",
    "Timed out",
    "Object not init",
    "Object init failed",
    "Operation already in progress",
    "Operation now in progress",
    "Object exist",
    "Object not exist",
    "Device or resource busy",
    "Device or resource full",
    "Device or resource empty",
    "Device or resource open failed",
    "Device or resource read failed",
    "Device or resource write failed",
    "Device or resource delete failed",

    /* just for call HIAI_ENGINE_LOG() to output log */
    "app log level: info",
    "app log level: warning",
    "app log level: error",
};

HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_OK, APP_LOG_STRING[APP_OK_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_FAIL, APP_LOG_STRING[APP_FAIL_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_INNER, APP_LOG_STRING[APP_INNER_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_POINTER, APP_LOG_STRING[APP_POINTER_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_INVALARG, APP_LOG_STRING[APP_INVALARG_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_NOTIMPL, APP_LOG_STRING[APP_NOTIMPL_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_OUTOFMEM, APP_LOG_STRING[APP_OUTOFMEM_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_BUFERROR, APP_LOG_STRING[APP_BUFERROR_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_PERM, APP_LOG_STRING[APP_PERM_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_TIMEOUT, APP_LOG_STRING[APP_TIMEOUT_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_NOTINIT, APP_LOG_STRING[APP_NOTINIT_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_INITFAIL, APP_LOG_STRING[APP_INITFAIL_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_ALREADY, APP_LOG_STRING[APP_ALREADY_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_INPROGRESS, APP_LOG_STRING[APP_INPROGRESS_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_EXIST, APP_LOG_STRING[APP_EXIST_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_NOTEXIST, APP_LOG_STRING[APP_NOTEXIST_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_BUSY, APP_LOG_STRING[APP_BUSY_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_FULL, APP_LOG_STRING[APP_FULL_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_EMPTY, APP_LOG_STRING[APP_EMPTY_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_OPENFAIL, APP_LOG_STRING[APP_OPENFAIL_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_READFAIL, APP_LOG_STRING[APP_READFAIL_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_WRITEFAIL, APP_LOG_STRING[APP_WRITEFAIL_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_DELFAIL, APP_LOG_STRING[APP_DELFAIL_CODE]);

HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_INFO, APP_INFO, APP_LOG_STRING[APP_INFO_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_WARNING, APP_WARNING, APP_LOG_STRING[APP_WARNING_CODE]);
HIAI_DEF_ERROR_CODE(MODID_APP_DEFINE, HIAI_ERROR, APP_ERROR, APP_LOG_STRING[APP_ERROR_CODE]);

static string GetAPPErrCodeInfo(uint32_t err)
{
    uint32_t errId = err & 0x0FFF;

    if (errId >= ERROR_CODE_NUM) {
        return "Error code unknown";
    } else {
        return APP_LOG_STRING[errId];
    }
}

#endif  // ERROR_CODE_H_
