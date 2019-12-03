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
#ifndef APP_COMMON_H
#define APP_COMMON_H

using namespace std;

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include "hiaiengine/data_type.h"
#include "hiaiengine/data_type_reg.h"
#include "dvpp/idvppapi.h"
#include "hiaiengine/ai_memory.h"
#include "Common.h"
#include "hiaiengine/log.h"


static const int32_t DECODE_IMAGE_NUM = 2;
static const int32_t DEFAULT_IMAGE_TYPE = -1;
static const int32_t TYPE_JPEG = 0;
static const int32_t TYPE_PNG = 1;
static const string IMAGE_TYPE[DECODE_IMAGE_NUM] = { "jpeg", "png" };
static const string EXTENSION_JPEG = "jpeg";
static const string EXTENSION_JPG = "jpg";
static const string EXTENSION_PNG = "png";

using hiai::BatchInfo;
using hiai::IMAGEFORMAT;
using hiai::ImageData;

typedef enum ImageType {
    IMAGE_TYPE_RAW = -1,
    IMAGE_TYPE_NV12 = 0,
    IMAGE_TYPE_JPEG,
    IMAGE_TYPE_PNG,
    IMAGE_TYPE_BMP,
    IMAGE_TYPE_TIFF,
    IMAGE_TYPE_VIDEO = 100
} ImageTypeT;

/**
* @brief: get the information file path in the dataset folder
* @[in]: value, path configuration string
* @[return]: string, info file path
 */
static std::string GetInfoFilePath(const std::string pathConfig)
{
    std::string datainfo_path = pathConfig;
    while (datainfo_path.back() == '/' || datainfo_path.back() == '\\') {
        datainfo_path.pop_back();
    }
    std::size_t tmp_ind =
        datainfo_path.find_last_of("/\\");
    std::string info_file = datainfo_path + "/" + "." + datainfo_path.substr(tmp_ind + 1) + "_data.info";
    return info_file;
}

typedef struct ImageInputInfo {
    std::string imageType;
    std::string filePath;
    std::string fileName;
} ImageInputInfoT;
template<class Archive>
void serialize(Archive &ar, ImageInputInfoT &data)
{
    ar(data.imageType, data.filePath, data.fileName);
}

#endif  // DVPPRESIZE_APPCOMMON_H




