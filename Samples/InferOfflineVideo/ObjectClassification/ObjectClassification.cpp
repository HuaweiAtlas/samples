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
#include "ObjectClassification.h"
#include "dvpp_utils.h"
#include "engine_tools.h"


HIAI_StatusT ObjectClassification::Init(const hiai::AIConfig& config,
    const std::vector<hiai::AIModelDescription>& model_desc)
{
    HIAI_StatusT ret = HIAI_OK;
    if (nullptr == modelManager) {
        modelManager = std::make_shared<hiai::AIModelManager>();
    }
    hiai::AIModelDescription modelDesc;
    loadModelDescription(config, modelDesc);
    // init ai model manager
    ret = modelManager->Init(config, { modelDesc });
    if (hiai::SUCCESS != ret) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[FaceDetection] ai model manager init failed!");
        return HIAI_ERROR;
    }
    // input/output buffer allocation
    std::vector<hiai::TensorDimension> inputTensorDims;
    std::vector<hiai::TensorDimension> outputTensorDims;
    ret = modelManager->GetModelIOTensorDim(modelDesc.name(), inputTensorDims, outputTensorDims);
    if (ret != hiai::SUCCESS) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[FaceDetection] hiai ai model manager init failed.");
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
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[FaceDetection] inputTensorDims.size() != 1 (%d vs. %d)",
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
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[FaceDetection] creat input tensors failed!");
        return HIAI_ERROR;
    }
    ret = creatIOTensors(modelManager, outputTensorDims, outputTensorVec, outputDataBuffer);
    if (HIAI_OK != ret) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "[FaceDetection] creat output tensors failed!");
        return HIAI_ERROR;
    }

    return ret;
}

void ObjectClassification::GetOutputResult(const std::vector<std::shared_ptr<hiai::IAITensor> >& outputTensorVec,
                                           DetectInfo& detectResult)
{
    for (int i = 0; i < outputTensorVec.size(); i++){
        std::shared_ptr<hiai::AISimpleTensor> result_tensor = std::static_pointer_cast<hiai::AISimpleTensor>(outputTensorVec[i]);
        const int size = result_tensor->GetSize() / (sizeof(float));
        // get the output data
        const float* data_ptr = (const float*) result_tensor->GetBuffer();
        // gat the index of target label
        int argmax = std::distance(data_ptr, std::max_element(data_ptr, data_ptr + size));
        detectResult.classifyResult.classIndex = argmax;
        detectResult.classifyResult.confidence = data_ptr[argmax];
    }

}

HIAI_IMPL_ENGINE_PROCESS("ObjectClassification", ObjectClassification, CLASSIFICATION_INPUT_SIZE)
{
    HIAI_StatusT ret = HIAI_OK;
    if (arg0 != nullptr) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "ObjectClassification received frame data");
        auto inputArg = std::static_pointer_cast<DeviceStreamData>(arg0);
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "widthAligned %d heightAligned %d", inputArg->imgOrigin.widthAligned,
                inputArg->imgOrigin.heightAligned);
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "width %d height %d", inputArg->imgOrigin.width, inputArg->imgOrigin.height);

        uint8_t* dataBufferPtr = inputDataBuffer[0].get();


        for (auto& det : inputArg->detectResult) {
             std::vector<DetectInfo> rect;
             rect.push_back(det);
            // crop and resize image by vpc
             vpcCropResize(inputArg->imgOrigin.buf.data.get(), inputArg->imgOrigin.width, inputArg->imgOrigin.height,
             dataBufferPtr, kWidth, kHeight, rect, INPUT_YUV420_SEMI_PLANNER_UV);

             hiai::AIContext aiContext;
             ret = modelManager->Process(aiContext, inputTensorVec, outputTensorVec, 0);
             if (hiai::SUCCESS != ret) {
                  HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "AI Model Manager Process failed");
                  return HIAI_ERROR;
             }

             //vector<OutputT> outputDataVec;
             //vector<ClassifyResultT> classifyResult;
             GetOutputResult(outputTensorVec,det);
         }

        //  inputArg->classifyResult = classifyResult;
        SendData(0, "DeviceStreamData", arg0);
    }
    return HIAI_OK;
}
