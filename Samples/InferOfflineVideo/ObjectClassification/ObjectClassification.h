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

#ifndef OBJECT_CLASSIFICATION_H
#define OBJECT_CLASSIFICATION_H

#include "hiaiengine/ai_model_manager.h"
#include "hiaiengine/engine.h"
#include "stream_data.h"

#define CLASSIFICATION_INPUT_SIZE 1
#define CLASSIFICATION_OUTPUT_SIZE 1

class ObjectClassification : public hiai::Engine {
public:
    ObjectClassification(){};

    HIAI_StatusT Init(const hiai::AIConfig& config,
        const std::vector<hiai::AIModelDescription>& model_desc);
    // Get output result
    void GetOutputResult(const std::vector<std::shared_ptr<hiai::IAITensor> >& outputTensorVec,
                         DetectInfo& detectResult);

    HIAI_DEFINE_PROCESS(CLASSIFICATION_INPUT_SIZE, CLASSIFICATION_OUTPUT_SIZE)

private:
    uint32_t kBatchSize = 1;
    uint32_t kChannel = 0;
    uint32_t kWidth = 0;
    uint32_t kHeight = 0;
    uint32_t kAlignedWidth = 0;
    uint32_t kAlignedHeight = 0;
    uint32_t kInputSize = 0;
    std::shared_ptr<hiai::AIModelManager> modelManager;
    std::vector<std::shared_ptr<uint8_t> > inputDataBuffer;
    std::vector<std::shared_ptr<uint8_t> > outputDataBuffer;
    std::vector<std::shared_ptr<hiai::IAITensor> > inputTensorVec;
    std::vector<std::shared_ptr<hiai::IAITensor> > outputTensorVec;
};

#endif // OBJECT_CLASSIFICATION_H