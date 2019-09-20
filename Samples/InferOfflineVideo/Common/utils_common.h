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
#ifndef ATLASFACEDEMO_UTILS_COMMON_H
#define ATLASFACEDEMO_UTILS_COMMON_H
#include <string>
#include <unordered_map>
#include <sys/stat.h>
#include "error_code.h"

typedef struct stat Stat;
using std::string;
using std::unordered_map;

inline unordered_map<string, string> kvmap(const hiai::AIConfig& config)
{
    unordered_map<string, string> kv;
    for (int index = 0; index < config.items_size(); ++index) {
        const ::hiai::AIConfigItem& item = config.items(index);
        kv.insert(std::make_pair(item.name(), item.value()));
    }
    return std::move(kv);
}

static std::vector<std::string> splitpath(
    const std::string& str, const std::set<char> delimiters)
{
    std::vector<std::string> result;
    char const* pch = str.c_str();
    char const* start = pch;
    for (; *pch; ++pch) {
        if (delimiters.find(*pch) != delimiters.end()) {
            if (start != pch) {
                std::string str(start, pch);
                result.push_back(str);
            } else {
                result.push_back("");
            }
            start = pch + 1;
        }
    }
    result.push_back(start);
    return result;
}

inline HIAI_StatusT saveFileBin(uint8_t* data_ptr, uint32_t data_len, const std::string& filename)
{
    FILE* fp = fopen(filename.c_str(), "wb");
    if (NULL == fp) {
        HIAI_ENGINE_LOG(HIAI_IDE_ERROR, "Save file engine: open file fail");
        fclose(fp);
        return HIAI_ERROR;
    } else {
        fwrite(data_ptr, 1, data_len, fp);
        fflush(fp);
        fclose(fp);
    }
    return HIAI_OK;
}

inline int mkdir_p(const std::string& dirname)
{
    Stat st;
    if (stat(dirname.c_str(), &st) != 0) {
        std::string cmd = "mkdir -p " + dirname;
        return system(cmd.c_str());
    }
}

template <class T>
inline string to_string(T __val, uint32_t n_zeros, char pchar = '0')
{
    std::string valstr = std::to_string(__val);
    int n_pads = n_zeros - valstr.length();
    return n_pads > 0 ? std::string(n_pads, pchar) + valstr : valstr;
}

#endif //ATLASFACEDEMO_UTILS_COMMON_H
