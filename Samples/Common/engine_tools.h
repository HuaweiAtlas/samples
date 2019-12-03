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
#ifndef ATLASFACEDEMO_ENGINE_TOOLS_H
#define ATLASFACEDEMO_ENGINE_TOOLS_H

#include "error_code.h"
#include "hiaiengine/ai_model_manager.h"
#include "hiaiengine/c_graph.h"
#include "hiaiengine/engine.h"
#include "hiaiengine/log.h"
#include "utils_common.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

using hiai::AIModelDescription;
using hiai::AIModelManager;
using hiai::AINeuralNetworkBuffer;
using hiai::AITensorDescription;
using hiai::AITensorFactory;
using hiai::IAITensor;
using hiai::TensorDimension;
using std::shared_ptr;
using std::string;

template <typename T>
struct array_deleter {
    void operator()(T const* p)
    {
        delete[] p;
    }
};

inline HIAI_StatusT checkEmpty(const string& value)
{
    if (value.empty()) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Check empty value!");
        return HIAI_ERROR;
    }
    return HIAI_OK;
}

inline HIAI_StatusT loadModelDescription(const hiai::AIConfig& config, hiai::AIModelDescription& modelDesc)
{
    auto kvcfg = kvmap(config);
    CHECK_RETURN_IF(kvcfg.count("model") <= 0);
    std::string modelPath = kvcfg["model"];
    std::set<char> delims{ '\\', '/' };
    std::vector<std::string> path = splitpath(modelPath, delims);
    std::string modelName = path.back();
    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "model path %s", modelPath.c_str());
    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "model name %s", modelName.c_str());
    modelDesc.set_path(modelPath);
    modelDesc.set_name(modelName);
    modelDesc.set_key(kvcfg["passcode"]);
    return HIAI_OK;
}

inline void logDumpDims(const hiai::TensorDimension& dims)
{
    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "tensor name %s dims size %d, ", dims.name.c_str(), dims.size);
    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "tensor dim n %d", dims.n);
    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "tensor dim c %d", dims.c);
    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "tensor dim h %d", dims.h);
    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "tensor dim w %d", dims.w);
}

static shared_ptr<IAITensor> createTensors(const TensorDimension& tensorDims, vector<shared_ptr<uint8_t> >& bufferVec)
{
    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[createTensors] tensor name %s size %d", tensorDims.name.c_str(), tensorDims.size);
    AITensorDescription tensorDesc = AINeuralNetworkBuffer::GetDescription();
    uint8_t* buffer = (uint8_t*)HIAI_DVPP_DMalloc(tensorDims.size);
    if (nullptr == buffer) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[createTensors] HIAI_DVPP_DMalloc failed.");
        return shared_ptr<hiai::IAITensor>(nullptr);
    }
    bufferVec.push_back(shared_ptr<uint8_t>(buffer, HIAI_DVPP_DFree));
    shared_ptr<IAITensor> outputTensor = AITensorFactory::GetInstance()->CreateTensor(
        tensorDesc, buffer, tensorDims.size);
    shared_ptr<hiai::AINeuralNetworkBuffer> nn_tensor = static_pointer_cast<hiai::AINeuralNetworkBuffer>(outputTensor);
    nn_tensor->SetName(tensorDims.name);
    nn_tensor->SetNumber(tensorDims.n);
    nn_tensor->SetChannel(tensorDims.c);
    nn_tensor->SetHeight(tensorDims.h);
    nn_tensor->SetWidth(tensorDims.w);
    nn_tensor->SetData_type(tensorDims.data_type);
    return outputTensor;
}

inline HIAI_StatusT creatIOTensors(const shared_ptr<AIModelManager>& modelManager, const vector<TensorDimension>& tensorDims, vector<shared_ptr<IAITensor> >& tensorVec, vector<shared_ptr<uint8_t> >& bufferVec)
{
    for (auto& tensorDim : tensorDims) {
        auto tensor = createTensors(tensorDim, bufferVec);
        if (nullptr == tensor) {
            return HIAI_ERROR;
        }
        tensorVec.push_back(tensor);
    }
    return HIAI_OK;
}

#endif //ATLASFACEDEMO_ENGINE_TOOLS_H
