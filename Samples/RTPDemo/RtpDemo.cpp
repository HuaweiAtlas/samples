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

/* 该函数用于演示Rtp接口的使用方法
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <memory.h>
#include <iostream>
#include <string>
#include <stdio.h>

#include "Rtp.h"
#include "TCPServer.h"

constexpr int SUCCESS = 0;
const int OP_FAILED = -1;
const int OPEN_FILE_FAILED = -10;

constexpr int MTU = 1500;
constexpr int MAX_FRAME_BUFFER = 4096 * 3122;
constexpr char CALLBACK_PYTHON_FILE[] = "/home/mss/python/pull_stream.py";


constexpr int MAX_LINE = 4096 * 8;
constexpr int BUFF_SIZE = 50;

std::unique_ptr<Atlas::RTP::Unpacking> unpack = nullptr;
std::unique_ptr<TCPServer> tcpServer = nullptr;
FILE* pFile = nullptr;
uint8_t* readbuff = nullptr;
uint8_t* h264Buffer = nullptr;
uint8_t* h265Buffer = nullptr;
int offset = 0;

std::string outFile = "/home/mmc/data/recv.h264";
std::string localIp = "127.0.0.1";
int localPort = 8888;

std::string GetLocalIp()
{
    return localIp;
}

int GetLocalPort()
{
    return localPort;
}

int InitUnpacking(std::unique_ptr<Atlas::RTP::Unpacking>& unpack) {
    std::map<uint8_t, std::string>payloadInfo;
    payloadInfo.emplace(96, "h264");
    payloadInfo.emplace(98, "h265");
    std::unique_ptr<Atlas::RTP::Unpacking> tmpUnpack(Atlas::RTP::RtpFactory::CreateUnpacking(payloadInfo));
    if (tmpUnpack == nullptr) {
        std::cout << "unpack init failed." << std::endl;
        return -1;
    }

    if (h264Buffer != nullptr || h265Buffer != nullptr)
    {
        return -1;
    }

    h264Buffer = (uint8_t*)malloc(MAX_FRAME_BUFFER);
    if (h264Buffer == nullptr) {
        return -1;
    }

    h265Buffer = (uint8_t*)malloc(MAX_FRAME_BUFFER);
    if (h265Buffer == nullptr) {
        free(h264Buffer);
        h264Buffer = nullptr;
        return -1;
    }

    tmpUnpack->SetFrameBuffer(96, h264Buffer, MAX_FRAME_BUFFER);
    tmpUnpack->SetFrameBuffer(98, h265Buffer, MAX_FRAME_BUFFER);

    //if (pFile == nullptr) {
        //pFile = fopen(outFile.c_str(), "wb");
    //}

    tmpUnpack->SetCallbackWrite([](uint8_t payload, bool isKeyFrame, uint8_t* data, size_t len){
        std::cout << "get a frame (payload=" << unsigned(payload) << ", isKeyFrame = " << isKeyFrame << ", len = " << len  << ")" << std::endl;
        //fwrite(data, sizeof(uint8_t), len, pFile);
    });
    

    unpack = std::move(tmpUnpack);

    return SUCCESS;
}


int HandleRecvBuffer(uint8_t* buffer, size_t len) {
    size_t rtpLen = 0;
    size_t remainLen = len;
    do {
        if (remainLen <= 4) {
            break;
        }

        if (unpack->CheckPkg(buffer, rtpLen)) {
            if (rtpLen + 4 < remainLen) {
                (void)unpack->InsertPkg(buffer, rtpLen+4);
                buffer += rtpLen + 4;
                remainLen -= rtpLen + 4;
            }
            else {
                break;
            }

        } else {
            ++buffer;
            --remainLen;
        }
    }while(remainLen > 0);
    return len - remainLen;
}


int InitTcpServer() {
    if (tcpServer != nullptr) {
        return SUCCESS;
    }
    std::unique_ptr<TCPServer> tmpTcpServer(new TCPServer(GetLocalIp(), GetLocalPort()));
    if (tmpTcpServer != nullptr) {
        if (readbuff == nullptr) {
            readbuff = new (std::nothrow) uint8_t[MAX_LINE];
            if (readbuff == nullptr) {
                std::cout << "new failed." << std::endl;
                return OP_FAILED;
            }
        }

        tmpTcpServer->SetCallbackRead([](int connFd){
            int len = recv(connFd, readbuff + offset, MAX_LINE - offset, 0);
            if (len == -1) {
                std::cout << "recv failed, connFd = " << connFd << ", err = " << strerror(errno) << std::endl;
                return;
            }

            len += offset;
            int delta = HandleRecvBuffer(readbuff, len);
            // update offset
            if (delta > 0) {
                memmove(readbuff, readbuff + delta, len - delta);
            }
            offset = len - delta;
            //std::cout << "cur recv len = " << len - offset << ", total len = " << len << ", deal len = " << delta << "remainLen =" << offset << std::endl;
            return;
        });

        if (!tmpTcpServer->Init()) {
            std::cout << "tcpServer init failed." << std::endl;
            return -1;
        }

        tcpServer = std::move(tmpTcpServer);
    }
    return SUCCESS;
}

int EnableMTSAndGetConnectInfo()
{
    std::string info;
    FILE* fd;
    int status = -1;
    size_t offset = 0;
    char buf[BUFF_SIZE] = {0};
    std::string channelName = "0000100";
    uint8_t h264Payload = 96;
    uint8_t h265Payload = 98;

    std::string commond = std::string("python3 ") + CALLBACK_PYTHON_FILE + " \"MSS\" " + " \"" + channelName + "\" " + std::to_string(h264Payload) + " " + std::to_string(h265Payload) + " " + GetLocalIp() + " " + std::to_string(GetLocalPort());
    if (NULL == (fd = popen(commond.c_str(), "r"))) {
        std::cout << "Receiver enable MTS failed.(open failed.)" << std::endl;
        return OPEN_FILE_FAILED;
    }

    while (fgets(buf, BUFF_SIZE, fd) != NULL) {
        std::string tmp(buf);
        info += tmp;
        (void)memset(buf, 0, BUFF_SIZE);
    }

    status = pclose(fd);

    if (WEXITSTATUS(status)) {
        if (status != 0) {
            std::cout <<"Receiver enable MTS failed.(err=" << info.c_str() << ")" << std::endl;
            return OP_FAILED;
        }
    }

    if (std::string::npos != (offset = info.find_last_of("\n"))) {
        (void)info.erase(offset, 1);
    }
    return SUCCESS;
}


int process() {    // init unpacking
    
    if (0 != InitUnpacking(unpack)) {
        return -1;
    }

    if (SUCCESS != InitTcpServer()){
        return -1;
    }

    EnableMTSAndGetConnectInfo();
    return 0;
}

void release() {    
    if (readbuff != nullptr) {
        delete[] readbuff;
        readbuff = nullptr;
    }

    if (h264Buffer != nullptr) {
        free(h264Buffer);
        h264Buffer = nullptr;
    }

    if (h265Buffer != nullptr) {
        free(h265Buffer);
        h265Buffer = nullptr;
    }

    //fclose(pFile);
    return;
}

int main(int argc, char* argv[])
{
    if (argc == 2) {
        localIp = argv[1];
    }
    else if (argc == 3) {
        localIp = argv[1];
        localPort = atoi(argv[2]);
    }
    else if (argc == 4) {
        localIp = argv[1];
        localPort = atoi(argv[2]);
        outFile = argv[3];
    }
    process();

    std::cout << "press n to quit..." << std::endl;
    while (getchar() != 'n') {
        ;
    }
    release();
    return 0;
}