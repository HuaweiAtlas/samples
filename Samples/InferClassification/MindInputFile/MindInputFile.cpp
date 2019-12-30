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
#include <memory>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <string>
#include <malloc.h>
#include <ctime>
#include <map>
#include "MindInputFile.h"
#include "hiaiengine/log.h"
#include "hiaiengine/data_type_reg.h"
#include "hiaiengine/ai_memory.h"
#include "AppCommon.h"
#include "FileManager.h"

using namespace std;

static const uint32_t BUFFER_LEN_OFFSET = 8;

// register EngineImageTransT
HIAI_REGISTER_SERIALIZE_FUNC("EngineImageTransT", EngineImageTransT, GetEngineImageTransPtr, GetEngineImageTransrPtr);

HIAI_StatusT MindInputFile::Init(const hiai::AIConfig &config,
                                 const std::vector<hiai::AIModelDescription> &model_desc)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[MindInputFile] start init!");
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[MindInputFile] end init!");
    return HIAI_OK;
}

// Engine process 
HIAI_IMPL_ENGINE_PROCESS("MindInputFile", MindInputFile, MIND_INPUT_SIZE)
{
    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[MindInputFile] start process!");
    printf("[MindInputFile] start process!\n");
    if (ctrlInfoT == nullptr) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[MindInputFile] input arg invaild");
        return HIAI_ERROR;
    }
    std::shared_ptr<ImageInputInfoT> imageInfo = std::static_pointer_cast<ImageInputInfoT>(arg0);

    std::shared_ptr<EngineImageTransT> tranData = std::make_shared<EngineImageTransT>();

    HIAI_StatusT ret;

    shared_ptr<FileManager> fileManager(new FileManager());

    FileInfo imageFileInfo = FileInfo();

    bool readRet = fileManager->ReadFileWithDmalloc(imageInfo->filePath, imageFileInfo);
    if (!readRet) {
        printf("Read ImageFile error.");
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Read ImageFile error.");
        return false;
    }
    uint32_t fileLen = imageFileInfo.size;
    uint32_t bufferLen = 0;
    if (IMAGE_TYPE[TYPE_JPEG] == imageInfo->imageType) {
		// when decoding jpeg, the buf len should 8 larger, the driver asked.  
		// Please refer to the DVPP manual for more details
        bufferLen = fileLen + BUFFER_LEN_OFFSET;
    } else {
        bufferLen = fileLen;
    }

    // when user send two pieces of data, bufferLenExtend is the size of the second piece, otherwise it is 0.
    uint32_t bufferLenExtend = 0;

    tranData->trans_buff = imageFileInfo.data;
    tranData->buffer_size = bufferLen;
    tranData->trans_buff_extend.reset(imageFileInfo.data.get() + bufferLen, DeleteNothing);
    tranData->buffer_size_extend = bufferLenExtend;

    if ("png" == imageInfo->imageType) {
        ret = SendData(0, "EngineImageTransT", tranData);
    } else {
        ret = SendData(1, "EngineImageTransT", tranData);
    }

    if (ret != HIAI_OK) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "SourceEngine send data error. ret = %d", ret);
        return ret;
    }

    HIAI_ENGINE_LOG(HIAI_IDE_INFO, "[MindInputFile] end process!");
    printf("[MindInputFile] end process!\n");
    return HIAI_OK;
}
