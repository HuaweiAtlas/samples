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
#ifndef CUSTOM_DATA_RECV_INTERFACE_H
#define CUSTOM_DATA_RECV_INTERFACE_H
#include <hiaiengine/api.h>
#include <string.h>

class CustomDataRecvInterface : public hiai::DataRecvInterface {
public:
    /**
    * @ingroup FasterRcnnDataRecvInterface
    * @brief init
    * @param [in]desc:std::string
    */
    CustomDataRecvInterface(const int graphID, const int chNum = 1):graphID(graphID), videoChNum(chNum)
    {
        if (videoChNum <= 0) {
            videoChNum = 1;
        }

        try {
            chFlag = new bool[videoChNum];
        } catch (bad_alloc &memExp) {
            chFlag = NULL;
        }

        for (int i = 0; i < videoChNum; ++i) {
            *(chFlag + i) = false;
        }
    }

    ~CustomDataRecvInterface()
    {
        if (chFlag != NULL) {
            delete[] chFlag;
        }
    }

    /**
    * @ingroup FasterRcnnDataRecvInterface
    * @brief RecvData RecvData
    * @param [in]
 */
    HIAI_StatusT RecvData(const std::shared_ptr<void> &message);

private:
    int graphID;
    bool *chFlag;
    int videoChNum;

    void SetChFlag(int channel)
    {
        if ((chFlag != NULL) && (channel < videoChNum)) {
            chFlag[channel] = true;
        }
    }

    bool GetGraphFlag(void)
    {
        bool flag = true;
        if (chFlag != NULL) {
            for (int i = 0; i < videoChNum; ++i) {
                flag = flag && chFlag[i];
            }
        } else {
            flag = false;
        }

        return flag;
    }
};

#endif  // MAIN_H
