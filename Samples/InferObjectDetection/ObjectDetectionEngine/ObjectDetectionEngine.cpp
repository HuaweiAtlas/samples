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
#include "ObjectDetectionEngine.h"
#include "engine_tools.h"
#include "error_code.h"
#include "hiaiengine/ai_memory.h"
#include "hiaiengine/c_graph.h"
#include "hiaiengine/data_type.h"
#include "hiaiengine/log.h"
#include <memory>

using std::map;
using std::shared_ptr;
using std::string;
using std::vector;

HIAI_REGISTER_DATA_TYPE("StreamInfo", StreamInfo);
HIAI_REGISTER_DATA_TYPE("ImageInfo", ImageInfo);
HIAI_REGISTER_DATA_TYPE("DetectInfo", DetectInfo);
HIAI_REGISTER_DATA_TYPE("DeviceStreamData", DeviceStreamData);

/* Read the input size of model from om file, for resize the input image in the DecodeEngine */
int g_detectInputWidth = 0;
int g_detectInputHeight = 0;
static const float THRESH = 0.8;
static const int32_t COL_SIZE = 7;

HIAI_StatusT ObjectDetectionEngine::Init(const hiai::AIConfig &config,
                                         const std::vector<hiai::AIModelDescription> &model_desc)
{
    HIAI_ENGINE_LOG(APP_INFO, "[ObjectDetectionEngine] start init!");
    HIAI_StatusT ret;
    /*
    * AI model maneger
 */
    if (modelManager == nullptr) {
        modelManager = std::make_shared<hiai::AIModelManager>();
    }
    hiai::AIModelDescription modelDesc;
    map<string, string> kvcfg = kvmap(config);
    if (HIAI_OK != checkEmpty(kvcfg["model"])) {
        return HIAI_ERROR;
    }
    std::string modelPath = kvcfg["model"];
    std::set<char> delims { '\\', '/' };
    std::vector<std::string> path = splitpath(modelPath, delims);
    std::string modelName = path.back();
    HIAI_ENGINE_LOG(APP_INFO, "model path %s", modelPath.c_str());
    HIAI_ENGINE_LOG(APP_INFO, "model name %s", modelName.c_str());
    modelDesc.set_path(modelPath);
    modelDesc.set_name(modelName);
    modelDesc.set_key(kvcfg["passcode"]);
    // init ai model manager
    ret = modelManager->Init(config, { modelDesc });
    if (hiai::SUCCESS != ret) {
        HIAI_ENGINE_LOG(APP_ERROR, "ai model manager init failed!");
        return HIAI_ERROR;
    }

    /*
    *  input/output buffer allocation
    */
    // get info of input and output
    std::vector<hiai::TensorDimension> inputTensorDims;
    std::vector<hiai::TensorDimension> outputTensorDims;
    ret = modelManager->GetModelIOTensorDim(modelName, inputTensorDims, outputTensorDims);
    if (ret != hiai::SUCCESS) {
        HIAI_ENGINE_LOG(APP_ERROR, "[ObjectDetectionEngine] GetModelIOTensorDim() failed.");
        return HIAI_ERROR;
    }
    // Get input image size form om file
    kHeight = inputTensorDims[0].h;
    kWidth = inputTensorDims[0].w;
    kChannel = inputTensorDims[0].c;
    kBatchSize = inputTensorDims[0].n;

    kAlignedWidth = ALIGN_UP(kWidth, ALIGN_128);
    kAlignedHeight = ALIGN_UP(kHeight, ALIGN_16);

    /* Read the input size of model from om file, for resize the input image in the DecodeEngine */
    g_detectInputWidth = kWidth;
    g_detectInputHeight = kHeight;

    for (auto &dims : inputTensorDims) {
        logDumpDims(dims);
    }
    for (auto &dims : outputTensorDims) {
        logDumpDims(dims);
    }

    // pre allocate input data buffer
    if (inputTensorDims.size() != 1) {
        HIAI_ENGINE_LOG(APP_ERROR, "[ObjectDetectionEngine] inputTensorDims.size() != 1");
        return HIAI_ERROR;
    }
    kInputSize = kBatchSize * kAlignedHeight * kAlignedWidth * YUV_BYTES;
    // todo
    // get ride off reading nchw from graph.config
    // we can get from input TensorDimensions
    if (kInputSize != inputTensorDims[0].size) {
        HIAI_ENGINE_LOG(APP_ERROR, "[ObjectDetectionEngine] inputSize != inputTensorDims[0].size (%d vs. %d)",
                        kInputSize, inputTensorDims[0].size);
        return HIAI_ERROR;
    }
    std::shared_ptr<uint8_t> inPtr(static_cast<uint8_t *>(HIAI_DVPP_DMalloc(kInputSize)), HIAI_DVPP_DFree);
    inputDataBuffer = std::make_pair(inPtr, kInputSize);
    hiai::AITensorDescription tensorDesc = hiai::AINeuralNetworkBuffer::GetDescription();
    std::shared_ptr<hiai::IAITensor> inputTensor = hiai::AITensorFactory::GetInstance()->CreateTensor(tensorDesc,
                                                   static_cast<void *>(inPtr.get()), kInputSize);
    inputTensorVec.push_back(inputTensor);

    // pre allocate output data buffer
    for (uint32_t index = 0; index < outputTensorDims.size(); index++) {
        hiai::AITensorDescription outputTensorDesc = hiai::AINeuralNetworkBuffer::GetDescription();
        HIAI_ENGINE_LOG(APP_INFO, "outputTensorDims[index].size %d", outputTensorDims[index].size);
        uint8_t *buf = static_cast<uint8_t *>(HIAI_DVPP_DMalloc(outputTensorDims[index].size));
        if (buf == nullptr) {
            HIAI_ENGINE_LOG(APP_ERROR, "[ObjectDetectionEngine] HIAI_DVPP_DMalloc failed.");
            return HIAI_ERROR;
        }
        outputDataBuffer.push_back(std::shared_ptr<uint8_t>(buf, HIAI_DVPP_DFree));
        std::shared_ptr<hiai::IAITensor> outputTensor = hiai::AITensorFactory::GetInstance()->CreateTensor( \
                                                        outputTensorDesc, buf, outputTensorDims[index].size);
        shared_ptr<hiai::AINeuralNetworkBuffer> nn_tensor = static_pointer_cast<hiai::AINeuralNetworkBuffer>( \
                                                            outputTensor);
        nn_tensor->SetName(outputTensorDims[index].name);
        outputTensorVec.push_back(outputTensor);
    }

    /* create the vpc object */
    if (piDvppApiVpc == NULL) {
        ret = CreateDvppApi(piDvppApiVpc);
        if ((ret != HIAI_OK) && (piDvppApiVpc == NULL)) {
            HIAI_ENGINE_LOG(APP_ERROR, "ObjectDetectionEngine fail to intialize vpc api!");
            return HIAI_ERROR;
        }
    }

    HIAI_ENGINE_LOG(APP_INFO, "ObjectDetectionEngine initial successfully!");
    return HIAI_OK;
}

ObjectDetectionEngine::~ObjectDetectionEngine()
{
    /* Destroy vpc object */
    if (piDvppApiVpc != NULL) {
        HIAI_ENGINE_LOG(APP_INFO, "Destroy vpc api!");
        DestroyDvppApi(piDvppApiVpc);
        piDvppApiVpc = NULL;
    }
}

HIAI_IMPL_ENGINE_PROCESS("ObjectDetectionEngine", ObjectDetectionEngine, OBJECT_DETECT_INPUT_SIZE)
{
    HIAI_ENGINE_LOG(APP_INFO, "ObjectDetectionEngine Process start");
    HIAI_StatusT ret = HIAI_OK;
    std::shared_ptr<DeviceStreamData> inputArg;
    if (nullptr != arg0) {
        inputArg = std::static_pointer_cast<DeviceStreamData>(arg0);

        if (inputArg->info.isEOS == 1) {
            HIAI_ENGINE_LOG(APP_INFO, "[ObjectDection] Video or jpg file end, clean up the buffer!");
        } else {
            inputArgQueue.push_back(inputArg);
            if (inputArgQueue.size() < kBatchSize) {
                HIAI_ENGINE_LOG(APP_INFO,
                                "Collecting batch data, in current, queue size %d", inputArgQueue.size());
                return HIAI_OK;
            }
        }
    } else {
        if (inputArgQueue.size() <= 0) {
            HIAI_ENGINE_LOG(APP_ERROR, "ObjectDetectionEngine get invalid input!");
            return HIAI_ERROR;
        }
        HIAI_ENGINE_LOG(APP_INFO, "inputArgQueue overtime!");
    }

    // resize yuv data to input size
    uint8_t *dataBufferPtr = inputDataBuffer.first.get();
    int buffLen = kInputSize;
    for (int i = 0; i < inputArgQueue.size(); i++) {
        std::shared_ptr<DeviceStreamData> temp = inputArgQueue[i];

        /* for C30 release */
        errno_t ret = memcpy_s(dataBufferPtr, buffLen, temp->detectImg.buf.data.get(),
                               temp->detectImg.buf.len_of_byte);
        if (ret != EOK) {
            HIAI_ENGINE_LOG(APP_ERROR, "memcpy_s of ObjectDetectionEngine is wrong");
            return HIAI_ERROR;
        }

        dataBufferPtr += temp->detectImg.buf.len_of_byte;
        buffLen -= temp->detectImg.buf.len_of_byte;
    }

    // inference
    HIAI_ENGINE_LOG(APP_INFO, "AI Model Manager Process Start!");
    hiai::AIContext aiContext;
    ret = modelManager->Process(aiContext, inputTensorVec, outputTensorVec, 0);
    if (hiai::SUCCESS != ret) {
        HIAI_ENGINE_LOG(APP_ERROR, "AI Model Manager Process failed");
        return HIAI_ERROR;
    }
    HIAI_ENGINE_LOG(APP_INFO, "AI Model Manager Process Finished!");

    // Post process: convert memory to Effective confidence, the output data will be saved into vector: inputArgQueue
    PostProcessDetectionSSD();

    // send data to next engine
    for (int32_t j = 0; j < inputArgQueue.size(); j++) {
        std::shared_ptr<DeviceStreamData> deviceStreamData = inputArgQueue[j];
        if (deviceStreamData->detectResult.size() > 0) {
            /* crop small image for follow up process */
            if (HIAI_OK != hiai::Engine::SendData(0, "DeviceStreamData",
                                                  std::static_pointer_cast<void>(deviceStreamData))) {
                HIAI_ENGINE_LOG(APP_ERROR, "ObjectDetectionEngine senddata error!");
                return HIAI_ERROR;
            }
            HIAI_ENGINE_LOG(APP_INFO, "ObjectDetectionEngine senddata successfully!");
        }
    }

    /* clear the inputArgQueue */
    inputArgQueue.clear();

    /* send */
    if (inputArg->info.isEOS == 1) {
        std::shared_ptr<DeviceStreamData> deviceStreamData = std::make_shared<DeviceStreamData>();
        deviceStreamData->info = inputArg->info;
        if (HIAI_OK != hiai::Engine::SendData(0, "DeviceStreamData",
                                              std::static_pointer_cast<void>(deviceStreamData))) {
            HIAI_ENGINE_LOG(APP_ERROR, "ObjectDetectionEngine senddata error!");
            return HIAI_ERROR;
        }
        HIAI_ENGINE_LOG(APP_INFO, "[ObjectDection] Video end, clean up the buffer, senddata flag !");
    }

    return HIAI_OK;
}

enum SSD_INDEX {BATCH_INDEX = 0, LABEL_INDEX = 1, SCORE_INDEX = 2, \
                XMIN_INDEX = 3, YMIN_INDEX = 4, XMAX_INDEX = 5, YMAX_INDEX = 6};

HIAI_StatusT ObjectDetectionEngine::PostProcessDetectionSSD(void)
{
    /*
    * tensor shape 200x7x1x1
    * for each row (7 elements), layout as follows
    * batch, label, score, xmin, ymin, xmax, ymax
    */
    shared_ptr<hiai::AINeuralNetworkBuffer> tensorResults = std::static_pointer_cast<hiai::AINeuralNetworkBuffer>
                                                            (outputTensorVec[0]);
    shared_ptr<hiai::AINeuralNetworkBuffer> tensorFaceNum = std::static_pointer_cast<hiai::AINeuralNetworkBuffer>
                                                            (outputTensorVec[1]);
    float *resStartPtr = static_cast<float *>(tensorResults->GetBuffer());
    // Don't use inputArgQueue.size() to instead of kBatchSize
    int numPerBatch = tensorResults->GetSize() / sizeof(float) / kBatchSize;
    float *validNumList = static_cast<float *>(tensorFaceNum->GetBuffer());
    if ((resStartPtr == NULL) || (validNumList == NULL)) {
        HIAI_ENGINE_LOG(APP_ERROR, "Result of inference is Null!");
        return HIAI_ERROR;
    }

    int validObjectCount = 0;
    for (int32_t j = 0; j < inputArgQueue.size(); j++) {
        HIAI_ENGINE_LOG(APP_INFO, "Numder of detected faces %d", (int32_t)validNumList[j]);
        std::shared_ptr<DeviceStreamData> deviceStreamData = inputArgQueue[j];
        float *resPtr = NULL;
        resPtr = resStartPtr + numPerBatch * j;

        /* parse the single batch result */
        validObjectCount = 0;
        for (int32_t i = 0; i < (int32_t)validNumList[j]; i++) {
            DetectInfo detectInfo;
            int32_t batch = (int32_t)(*resPtr);
            detectInfo.classId = (int32_t)(*(resPtr + LABEL_INDEX));
            detectInfo.confidence = *(resPtr + SCORE_INDEX);
            detectInfo.location.anchor_lt.x = (int32_t)(*(resPtr + XMIN_INDEX) * deviceStreamData->imgOrigin.width);
            detectInfo.location.anchor_lt.y = (int32_t)(*(resPtr + YMIN_INDEX) * deviceStreamData->imgOrigin.height);
            detectInfo.location.anchor_rb.x = (int32_t)(*(resPtr + XMAX_INDEX) * deviceStreamData->imgOrigin.width);
            detectInfo.location.anchor_rb.y = (int32_t)(*(resPtr + YMAX_INDEX) * deviceStreamData->imgOrigin.height);
            if ((detectInfo.confidence > THRESH) && (detectInfo.classId > 0)) {
                validObjectCount++;
                HIAI_ENGINE_LOG(APP_INFO, "batch %d, label %d, score %f, xmin %d, ymin %d, xmax %d, ymax %d",
                                batch, detectInfo.classId, detectInfo.confidence,
                                detectInfo.location.anchor_lt.x, detectInfo.location.anchor_lt.y,
                                detectInfo.location.anchor_rb.x, detectInfo.location.anchor_rb.y);
                /* output detect information */
                deviceStreamData->detectResult.push_back(detectInfo);
            }
            resPtr += COL_SIZE;
        }
        HIAI_ENGINE_LOG(APP_INFO, "validObjectCount %d", validObjectCount);
    }

    return HIAI_OK;
}
