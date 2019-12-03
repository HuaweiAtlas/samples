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

#include "StreamPuller.h"
#include "SampleMemory.h"
#include "error_code.h"
#include "hiaiengine/ai_memory.h"
#include "utils_common.h"
#include <chrono>

std::shared_ptr<AVFormatContext> createFormatContext(const std::string& streamName)
{
    AVFormatContext* formatContext = nullptr;
    AVDictionary* options = nullptr;
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    av_dict_set(&options, "stimeout", "3000000", 0);
    int ret = avformat_open_input(&formatContext, streamName.c_str(), nullptr, &options);
    if (nullptr != options) {
        av_dict_free(&options);
    }
    if (ret != 0) {
        printf("Couldn't open input stream %s, ret=%d\n", streamName.c_str(), ret);
        return nullptr;
    }
    ret = avformat_find_stream_info(formatContext, nullptr);
    if (ret != 0) {
        printf("Couldn't find stream information\n");
        return nullptr;
    }
    return std::shared_ptr<AVFormatContext>(formatContext,
        [](AVFormatContext* p) { if (p) avformat_close_input(&p); });
}

StreamPuller::~StreamPuller()
{
    stopStream();
}

void StreamPuller::getStreamInfo()
{
    if (pFormatCtx != nullptr) {
        videoIndex = -1;
        AVCodecID codecId = pFormatCtx->streams[0]->codecpar->codec_id;
        if (codecId == AV_CODEC_ID_H264) {
            format = H264;
        } else if (codecId == AV_CODEC_ID_H265) {
            format = H265;
        } else {
            printf("\033[0;31mError unsupported format %d\033[0m\n", codecId);
            return;
        }
        for (int i = 0; i < pFormatCtx->nb_streams; i++) {
            AVStream* inStream = pFormatCtx->streams[i];
            if (inStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                videoIndex = i;
                mHeight = inStream->codecpar->height;
                mWidth = inStream->codecpar->width;                
            } else {
                printf("Error codec_type %d\n", inStream->codecpar->codec_type);
            }
        }
        if (videoIndex == -1) {
            printf("Didn't find a video stream\n");
        }
    }
}

HIAI_StatusT StreamPuller::SendDataToNextEngin(const uint32_t eos)
{
    std::shared_ptr<StreamRawData> output = std::make_shared<StreamRawData>();
    output->buf = dataBuffer;
    output->info.channelId = channelId;
    output->info.format = format;
    output->info.isEOS = eos;

    HIAI_StatusT ret = SendData(0, "StreamRawData", std::static_pointer_cast<void>(output));
    
    return ret;
}

void StreamPuller::pullStreamDataLoop()
{
    AVPacket pkt;
    while (1) {
        if (stop || pFormatCtx == nullptr) {
            break;
        }
        av_init_packet(&pkt);
        int ret = av_read_frame(pFormatCtx.get(), &pkt);
        if (ret != 0) {
            printf("[StreamPuller] channel %d Read frame failed, continue!\n", channelId);
            if (ret == AVERROR_EOF) {
                printf("[StreamPuller] channel %d StreamPuller is EOF, over!\n", channelId);
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        } else if (pkt.stream_index == videoIndex) {
            if (pkt.size <= 0) {
                printf("channel %d Invalid pkt.size %d\n", channelId, pkt.size);
                continue;
            }

            uint8_t* buffer = (uint8_t*)Sample_DMalloc(pkt.size);

            if (buffer == NULL) {
                printf("channel %d Sample_DMalloc buffer faild\n", channelId);
                av_packet_unref(&pkt);
                break;
            }

            memcpy_s(buffer, pkt.size, pkt.data, pkt.size);
            dataBuffer.data = std::shared_ptr<uint8_t>(buffer, Sample_DFree);
            dataBuffer.len_of_byte = pkt.size;
            blockId++;

            HIAI_StatusT ret = SendDataToNextEngin(0);
            if (ret != HIAI_OK) {
                printf("channel %d StreamPuller send data failed %d\n", channelId, ret);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            } else {
                printf("channel %d pkt.size %d\n", channelId, pkt.size);
            }
            av_packet_unref(&pkt);
        }
    }

    HIAI_StatusT ret = SendDataToNextEngin(1);
    if (ret != HIAI_OK) {
        printf("channel %d StreamPuller send data failed %d\n", channelId, ret);
    }
    printf("channel %d pullStreamDataLoop end of stream\n", channelId);
    av_init_packet(&pkt);
    stop = 1;
}

void StreamPuller::stopStream()
{
    stop = 1;
    if (sendDataRunner.joinable()) {
        sendDataRunner.join();
    }
}

HIAI_StatusT StreamPuller::startStream(const string& streamName)
{
    stopStream();
    stop = 0;
    pFormatCtx = createFormatContext(streamName);
    if (pFormatCtx == nullptr) {
        return HIAI_ERROR;
    }
    // for debug dump
    av_dump_format(pFormatCtx.get(), 0, streamName.c_str(), 0);
    // get stream infomation
    getStreamInfo();
    // set up loop threads
    sendDataRunner = std::thread(&StreamPuller::pullStreamDataLoop, this);
    return HIAI_OK;
}

HIAI_StatusT StreamPuller::Init(const hiai::AIConfig& config, const std::vector<hiai::AIModelDescription>& model_desc)
{
    auto aimap = kvmap(config);
    if (aimap.count("format")) {
        if (aimap["format"] == "h264") {
            format = H264;
        } else {
            format = H265;
        }
    }
    CHECK_RETURN_IF(aimap.count("channel_id") <= 0);
    channelId = std::stoi(aimap["channel_id"]);
    avformat_network_init();

    if (aimap.count("stream_name")) {
        streamName = aimap["stream_name"];
    }

    return HIAI_OK;
}

HIAI_IMPL_ENGINE_PROCESS("StreamPuller", StreamPuller, RP_INPUT_SIZE)
{
    if (nullptr != arg0) {
        shared_ptr<string> inputArg = std::static_pointer_cast<string>(arg0);
        if (!inputArg->empty()) {
            streamName = *inputArg;
        }
        startStream(streamName);
    }
    return HIAI_OK;
}