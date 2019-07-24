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

#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "TCPServer.h"

TCPServer::TCPServer(const std::string ip, const int port)
 : localIp(ip), localPort(port) {
}

TCPServer::~TCPServer() {
    Stop();

    for (auto& connFd : connFdSet) {
        ::close(connFd);
    }
    connFdSet.clear();

    if (sockFd >= 0) {
        ::close(sockFd);
        sockFd = -1;
    }

    if (epollFd >= 0) {
        ::close(epollFd);
        epollFd = -1;
    }
}

bool TCPServer::Init() {
    //
    if (epollFd < 0) {
        epollFd = epoll_create1(0);
        if (epollFd < 0) {
            std::cout << "create epoll failed (" << strerror(errno) << ")" <<std::endl;
            return false;
        }
    }

    if (sockFd < 0) {
        sockFd = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
        if (sockFd < 0) {
            std::cout << "TcpServer create socket failed.(" << strerror(errno) << std::endl;
            return false;
        }
        // set non-blocking mode
        if (::fcntl(sockFd, F_SETFL, (unsigned int)::fcntl(sockFd, F_GETFD, 0) | O_NONBLOCK) == -1) {
            std::cout << "TcpServer set nonblocking failed." << std::endl;
            ::close(sockFd);
            sockFd = -1;
            return false;
        }

        // bind socket
        struct sockaddr_in serverAddr;
        (void)memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(localIp.c_str());
        serverAddr.sin_port = htons(localPort);
        int ret = ::bind(sockFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        if (ret < 0)
        {
            std::cout << "Tcp server(addr: " << localIp << ":" << localPort << ")  bind failed.(" <<  strerror(errno) << ")" << std::endl;
            ::close(sockFd);
            sockFd = -1;
            return false;
        }

        ret = ::listen(sockFd, 10);
        if (ret < 0) {
            std::cout << "Tcp server(addr: " << localIp << ":" << localPort << ")  listen failed.(" <<  strerror(errno) << ")" << std::endl;
            ::close(sockFd);
            sockFd = -1;
            return false;
        }
        std::cout << "Tcp server is listen on " << localIp << ":" << localPort << " success." << std::endl;
    }

    // register sockFd on epollFd
    struct epoll_event event = {0};
    event.events = EPOLLIN;
    event.data.fd = sockFd; // ptr 与fd 公用
    int ret = ::epoll_ctl(epollFd, EPOLL_CTL_ADD, event.data.fd, &event);
    if (ret == -1)
    {
        std::cout << "EpollScheduler add session(epfd=" << epollFd << ", fd=" << event.data.fd << ", " << strerror(errno) << ") failed." << std::endl;
        return false;
    }

    return true;
}

bool TCPServer::Start() {
    if (isStop) {
        isStop = false;
        std::thread Thread(&TCPServer::EventLoop, this);
        threadHeadle = Thread.native_handle();
        Thread.detach();
        std::cout << "Epoll scheduler start success." << std::endl;
    }
    return true;
}

void TCPServer::EventLoop() {
    struct epoll_event events[512];
    while (!isStop) {
        int eventNum = epoll_wait(epollFd, events, 512, -1);
        if (eventNum < 0) {
            if (errno != EINTR) {
                break;
            }
            continue;
        }

        for (int i=0; i < eventNum; i++) {
            if (events[i].data.fd == sockFd) {
                HandleAccept();
            }
            else {
                callbackRead(events[i].data.fd);
            }
        }
    }
}


void TCPServer::Stop() {
    if (!isStop) {
        isStop = true;

        struct epoll_event event = {0};
        for (auto& connFd : connFdSet) {
            ::epoll_ctl(epollFd, EPOLL_CTL_DEL, connFd, &event);
            ::close(connFd);
        }
        connFdSet.clear();
        ::epoll_ctl(epollFd, EPOLL_CTL_DEL, sockFd, &event);
        pthread_cancel(threadHeadle);
    }
}


void TCPServer::HandleAccept() {
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(struct sockaddr_in);
    int connFd = ::accept(sockFd, (struct sockaddr*)&addr, &addrLen);
    if (connFd < 0) {
        std::cout << "Tcp server(addr: " << localIp << ":" << localPort << ")  accept failed.(" <<  strerror(errno) << ")" << std::endl;
        return;
    }

    std::cout << "Tcp server(addr: " << localIp << ":" << localPort << ")  accept connection from " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << ".(connFd=" << connFd << ")" << std::endl;

    if (::fcntl(connFd, F_SETFL, (unsigned int)::fcntl(connFd, F_GETFD, 0) | O_NONBLOCK) == -1) {
        std::cout << "TcpServer(connFd=" << connFd << ") set nonblocking failed." << std::endl;
        ::close(connFd);
    }

    struct epoll_event event = {0};
    event.events = EPOLLIN;
    event.data.fd = connFd; // ptr 与fd 公用
    int ret = ::epoll_ctl(epollFd, EPOLL_CTL_ADD, event.data.fd, &event);
    if (ret == -1)
    {
        std::cout << "EpollScheduler add session(connFd=" << connFd << ", fd=" << event.data.fd << ", " << strerror(errno) << ") failed." << std::endl;
        return;
    }
    connFdSet.insert(connFd);
    return;
}