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

#ifndef DVPPANDOPENCV_FILEMANAGER_H
#define DVPPANDOPENCV_FILEMANAGER_H

#include <unistd.h>
#include <fstream>
#include <algorithm>
#include <vector>
#include <set>
#include <dirent.h>
#include <string.h>
#include <memory>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "hiaiengine/ai_memory.h"

using namespace std;

typedef struct FileInfo {
    uint32_t size;
    std::shared_ptr<uint8_t> data;
} FileInfoT;

class FileManager {
public:
    bool ExistFile(const string &dirPath);
    bool ExistDir(const string &dirPath);
    bool CreateDir(const string &dirPath);
    bool CreateFile(const string &dirPath);
    bool ReadFile(const string &dirPath, FileInfo &fileData);
    vector<string> ReadByExtension(const string &dirPath, const vector<string> format);
    string GetExtension(const string &dirPath);
    string GetName(const string &dirPath);
    string GetParent(const string &dirPath);
    bool ChangeDir(const char *dirPath);
    std::vector<std::string> SplitPath(const std::string &str, const std::set<char> delimiters);
    bool ReadFileWithDmalloc(const string &dirPath, FileInfo &fileInfo);
};
#endif  // DVPPANDOPENCV_FILEMANAGER_H
