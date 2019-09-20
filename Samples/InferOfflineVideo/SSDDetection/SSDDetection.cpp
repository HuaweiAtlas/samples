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
#include "SSDDetection.h"
#include "dvpp_utils.h"
#include "engine_tools.h"
#include "error_code.h"
#include "hiaiengine/ai_memory.h"
#include "hiaiengine/c_graph.h"
#include "hiaiengine/data_type.h"
#include "hiaiengine/log.h"
#include "opencv2/opencv.hpp"
#include <memory>

using std::map;
using std::shared_ptr;
using std::string;
using std::vector;

HIAI_StatusT SSDDetection::Init(const hiai::AIConfig& config,
    const std::vector<hiai::AIModelDescription>& model_desc)
{
    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[SSDDetection] start init!");
    HIAI_StatusT ret = HIAI_OK;
    if (nullptr == modelManager) {
        modelManager = std::make_shared<hiai::AIModelManager>();
    }
    hiai::AIModelDescription modelDesc;
    loadModelDescription(config, modelDesc);
    // init ai model manager
    ret = modelManager->Init(config, { modelDesc });
    if (hiai::SUCCESS != ret) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[SSDDetection] ai model manager init failed!");
        return HIAI_ERROR;
    }
    // input/output buffer allocation
    std::vector<hiai::TensorDimension> inputTensorDims;
    std::vector<hiai::TensorDimension> outputTensorDims;
    ret = modelManager->GetModelIOTensorDim(modelDesc.name(), inputTensorDims, outputTensorDims);
    if (ret != hiai::SUCCESS) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[SSDDetection] hiai ai model manager init failed.");
        return HIAI_ERROR;
    }
    for (auto& dims : inputTensorDims) {
        logDumpDims(dims);
    }
    for (auto& dims : outputTensorDims) {
        logDumpDims(dims);
    }
    // input dims
    if (1 != inputTensorDims.size()) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[SSDDetection] inputTensorDims.size() != 1 (%d vs. %d)",
            inputTensorDims.size(), 1);
        return HIAI_ERROR;
    }

    kBatchSize = inputTensorDims[0].n;
    kChannel = inputTensorDims[0].c;
    kHeight = inputTensorDims[0].h;
    kWidth = inputTensorDims[0].w;
    kInputSize = inputTensorDims[0].size;
    kAlignedWidth = ALIGN_UP(kWidth, DVPP_STRIDE_WIDTH);
    kAlignedHeight = ALIGN_UP(kHeight, DVPP_STRIDE_HEIGHT);
    ret = creatIOTensors(modelManager, inputTensorDims, inputTensorVec, inputDataBuffer);
    if (HIAI_OK != ret) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[SSDDetection] creat input tensors failed!");
        return HIAI_ERROR;
    }
    ret = creatIOTensors(modelManager, outputTensorDims, outputTensorVec, outputDataBuffer);
    if (HIAI_OK != ret) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[SSDDetection] creat output tensors failed!");
        return HIAI_ERROR;
    }
    HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[SSDDetection] end init!");
    return HIAI_OK;
}

HIAI_IMPL_ENGINE_PROCESS("SSDDetection", SSDDetection, DT_INPUT_SIZE)
{
    HIAI_StatusT ret = HIAI_OK;
    std::shared_ptr<DeviceStreamData> inputArg = std::static_pointer_cast<DeviceStreamData>(arg0);
    if (nullptr == inputArg) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Fail to process invalid message");
        return HIAI_ERROR;
    }

    inputArgQueue.push_back(std::move(inputArg));
    // waiting for batch data
    if (inputArgQueue.size() < kBatchSize) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR,
            "Collecting batch data, in queue, current size %d", inputArgQueue.size());
        return HIAI_OK;
    }
    // resize yuv data to input size
    uint8_t* dataBufferPtr = inputDataBuffer[0].get();
    for (int i = 0; i < inputArgQueue.size(); i++) {
        inputArg = inputArgQueue[i];
        char outFilename[128];
        time_pair vpcStamps;
        vpcResize(inputArg->imgOrigin.buf.data.get(), inputArg->imgOrigin.width, inputArg->imgOrigin.height,
            dataBufferPtr, kWidth, kHeight);
        dataBufferPtr += kInputSize;
    }
    // inference
    hiai::AIContext aiContext;
    time_pair process;
    ret = modelManager->Process(aiContext, inputTensorVec, outputTensorVec, 0);
    if (hiai::SUCCESS != ret) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "AI Model Manager Process failed");
        return HIAI_ERROR;
    }

    postProcessDetection();
    inputArgQueue.clear();

    return HIAI_OK;
}

HIAI_StatusT SSDDetection::postProcessDetection()
{
    // tensor shape 200x7x1x1
    // for each row (7 elements), layout as follows
    // batch, label, score, xmin, ymin, xmax, ymax
    shared_ptr<hiai::AINeuralNetworkBuffer> tensorResults = std::static_pointer_cast<hiai::AINeuralNetworkBuffer>(outputTensorVec[0]);
    shared_ptr<hiai::AINeuralNetworkBuffer> tensorObjNum = std::static_pointer_cast<hiai::AINeuralNetworkBuffer>(outputTensorVec[1]);
    int objNum = (int)(*(float*)tensorObjNum->GetBuffer());
    const float thresh = 0.5;
    const int colSize = 7;
    int validFaceCount = 0;
    float* resPtr = (float*)tensorResults->GetBuffer();
    for (int i = 0; i < objNum; i++) {
        float score = *(resPtr + 2);
        if (score > thresh) {
            int batch = (int)(*resPtr);
            int label = (int)(*(resPtr + 1));
            float xmin = *(resPtr + 3);
            float ymin = *(resPtr + 4);
            float xmax = *(resPtr + 5);
            float ymax = *(resPtr + 6);
            validFaceCount++;
            // batch, label, score, xmin, ymin, xmax, ymax);
            shared_ptr<DeviceStreamData>* streamPtr = &inputArgQueue[batch];
            const uint32_t img_width = (*streamPtr)->imgOrigin.width;
            const uint32_t img_height = (*streamPtr)->imgOrigin.height;
            DetectInfo info;
            info.classId = label;
            info.location.anchor_lt.x = std::max(xmin, 0.f) * img_width;
            info.location.anchor_lt.y = std::max(ymin, 0.f) * img_height;
            info.location.anchor_rb.x = std::min(xmax, 1.f) * img_width;
            info.location.anchor_rb.y = std::min(ymax, 1.f) * img_height;
            info.confidence = score;
            (*streamPtr)->detectResult.push_back(info);
        }
        resPtr += colSize;
    }
    for (auto& outputData : inputArgQueue) {
        HIAI_StatusT ret = SendData(0, "DeviceStreamData", std::static_pointer_cast<void>(outputData));
        if (HIAI_OK != ret) {
            HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "SSDDetection send data failed");
        }
    }

    return HIAI_OK;
}
