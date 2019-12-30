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
#include "FileManager.h"
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <securec.h>

using namespace std;
#define BUF_SIZE 32U
const int FILE_MODE = 777;
const int TWO = 2;
static const uint32_t DEMALLOC_TIMEOUT = 10000;
static const string SLASH = "/";
bool FileManager::ExistFile(const string &dirPath)
{
    struct stat fileSat;
    char c[PATH_MAX + 1] = { 0x00 };
    errno_t err = strcpy_s(c, PATH_MAX + 1, dirPath.c_str());
    if (err != EOK) {
        printf("[ERROR] strcpy %s failed!\n", c);
        return false;
    }

    char path[PATH_MAX + 1] = { 0x00 };
    if ((strlen(c) > PATH_MAX) || (realpath(c, path) == NULL)) {
        printf("Get canonnicalize path fail!\n");
        return false;
    }
    if (stat(c, &fileSat) == 0 && S_ISREG(fileSat.st_mode)) {
        return true;
    }
    return false;
}

bool FileManager::ExistDir(const string &dirPath)
{
    char c[PATH_MAX + 1] = { 0x00 };
    errno_t err = strcpy_s(c, PATH_MAX + 1, dirPath.c_str());
    if (err != EOK) {
        printf("[ERROR] strcpy %s failed!\n", c);
        return false;
    }
    char path[PATH_MAX + 1] = { 0x00 };
    if ((strlen(c) > PATH_MAX) || (realpath(c, path) == NULL)) {
        printf("Get canonnicalize path fail!\n");
        return false;
    }
    DIR *dir = opendir(c);
    if (dir != NULL) {
        closedir(dir);
        return true;
    } else {
        return false;
    }
}

bool FileManager::FileManager::CreateDir(const string &dirPath)
{
    char c[PATH_MAX + 1] = { 0x00 };
    errno_t err = strcpy_s(c, PATH_MAX + 1, dirPath.c_str());
    if (err != EOK) {
        printf("[ERROR] strcpy %s failed!\n", c);
        return false;
    }
    char path[PATH_MAX + 1] = { 0x00 };
    if ((strlen(c) > PATH_MAX) || (realpath(c, path) == NULL)) {
        printf("Get canonnicalize path fail!\n");
        return false;
    }
    int dirExist = access(c, W_OK);
    if (-1 == dirExist) {
        if (mkdir(c, FILE_MODE) == -1) {
            return false;
        }
    }
    return true;
}

bool FileManager::CreateFile(const string &dirPath)
{
    char c[PATH_MAX + 1] = { 0x00 };
    errno_t err = strcpy_s(c, PATH_MAX + 1, dirPath.c_str());
    if (err != EOK) {
        printf("[ERROR] strcpy %s failed!\n", c);
        return false;
    }

    char path[PATH_MAX + 1] = { 0x00 };
    if ((strlen(c) > PATH_MAX) || (realpath(c, path) == NULL)) {
        printf("Get canonnicalize path fail!\n");
        return false;
    }

    std::string pathStr(path, path + strlen(path));
    if (!ExistFile(pathStr)) {
        if (FILE *file = fopen(path, "wb")) {
            fclose(file);
            return true;
        }
    }
    return false;
}

bool FileManager::ReadFile(const string &dirPath, FileInfo &fileData)
{
    char c[PATH_MAX + 1] = { 0x00 };
    errno_t err = strcpy_s(c, PATH_MAX + 1, dirPath.c_str());
    if (err != EOK) {
        printf("[ERROR] strcpy %s failed!\n", c);
        return false;
    }

    char path[PATH_MAX + 1] = { 0x00 };
    if ((strlen(c) > PATH_MAX) || (realpath(c, path) == NULL)) {
        printf("Get canonnicalize path fail!\n");
        return false;
    }
    FILE *fp = fopen(path, "rb");
    if (NULL == fp) {
        printf("Open file failed!");
        return false;
    }
    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (fileSize > 0) {
        fileData.size = fileSize;
        fileData.data = shared_ptr<uint8_t>(new uint8_t[fileSize]);
        uint32_t readRet = fread(fileData.data.get(), 1, fileSize, fp);
        if (readRet <= 0) {
            if (fileData.data.get() != nullptr) {
                delete fileData.data.get();
            }
            fclose(fp);
            return false;
        }
        fclose(fp);
        return true;
    }
    fclose(fp);
    return false;
}

bool FileManager::ReadFileWithDmalloc(const string &dirPath, FileInfo &fileInfo)
{
    char c[PATH_MAX + 1] = { 0x00 };
    errno_t err = strcpy_s(c, PATH_MAX + 1, dirPath.c_str());
    if (err != EOK) {
        printf("[ERROR] strcpy %s failed!\n", c);
        return false;
    }
    char path[PATH_MAX + 1] = { 0x00 };
    if ((strlen(c) > PATH_MAX) || (realpath(c, path) == NULL)) {
        printf("Get canonnicalize path fail!\n");
        return false;
    }
    FILE *fp = fopen(path, "rb");
    if (fp == nullptr) {
        printf("Open file failed!");
        return false;
    }
    fseek(fp, 0, SEEK_END);
    long int fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (fileSize > 0) {
        uint8_t *buffer = nullptr;
        HIAI_StatusT ret = hiai::HIAIMemory::HIAI_DMalloc(fileSize, (void *&)buffer, DEMALLOC_TIMEOUT);
        if (ret != HIAI_OK) {
            if (buffer != nullptr) {
                hiai::HIAIMemory::HIAI_DFree(buffer);
            }
            printf("Dmalloc Fail.");
            fclose(fp);
            return false;
        }
        if (buffer == nullptr) {
            printf("Dmalloc Fail.");
            fclose(fp);
            return false;
        }
        size_t len = fread(buffer, 1, fileSize, fp);
        if (len < 0) {
			hiai::HIAIMemory::HIAI_DFree(buffer);
            fclose(fp);
            return false;
        }
        fileInfo.size = fileSize;
        fileInfo.data = shared_ptr<uint8_t>(buffer, [](uint8_t *p) { hiai::HIAIMemory::HIAI_DFree(p); });
        fclose(fp);
        return true;
    }
    fclose(fp);
    return false;
}

// this only for jpeg decode
bool FileManager::ReadFileWithDmallocOffset(const string &dirPath, FileInfo &fileInfo, const uint32_t offSet)
{
    char c[PATH_MAX + 1] = { 0x00 };
    errno_t err = strcpy_s(c, PATH_MAX + 1, dirPath.c_str());
    if (err != EOK) {
        printf("[ERROR] strcpy %s failed!\n", c);
        return false;
    }
    char path[PATH_MAX + 1] = { 0x00 };
    if ((strlen(c) > PATH_MAX) || (realpath(c, path) == NULL)) {
        printf("Get canonnicalize path fail!\n");
        return false;
    }
    FILE *fp = fopen(path, "rb");
    if (fp == nullptr) {
        printf("Open file failed!");
        return false;
    }
    fseek(fp, 0, SEEK_END);
    long int fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (fileSize > 0) {
        uint8_t *buffer = nullptr;
		// when decoding jpeg, the buf len should 8 larger, the driver asked.  
		// Please refer to the DVPP manual for more details
        HIAI_StatusT ret = hiai::HIAIMemory::HIAI_DMalloc(fileSize + offSet, (void *&)buffer, DEMALLOC_TIMEOUT);
        if (ret != HIAI_OK) {
            if (buffer != nullptr) {
                hiai::HIAIMemory::HIAI_DFree(buffer);
            }
            printf("Dmalloc Fail.");
            fclose(fp);
            return false;
        }
        if (buffer == nullptr) {
            printf("Dmalloc Fail.");
            fclose(fp);
            return false;
        }
        size_t len = fread(buffer, 1, fileSize, fp);
        if (len < 0) {
			hiai::HIAIMemory::HIAI_DFree(buffer);
            fclose(fp);
            return false;
        }
        fileInfo.size = fileSize;
        fileInfo.data = shared_ptr<uint8_t>(buffer, [](uint8_t *p) { hiai::HIAIMemory::HIAI_DFree(p); });
        fclose(fp);
        return true;
    }
    fclose(fp);
    return false;
}

string FileManager::GetExtension(const string &filePath)
{
    std::set<char> delims { '.' };
    std::vector<std::string> path = SplitPath(filePath, delims);
    return path[path.size() - 1];
}

vector<string> FileManager::ReadByExtension(const string &dirPath, const vector<string> format)
{
    vector<string> fileToRead;
    char c[PATH_MAX + 1] = { 0x00 };
    errno_t err = strcpy_s(c, PATH_MAX + 1, dirPath.c_str());
    if (err != EOK) {
        printf("[ERROR] strcpy %s failed!\n", c);
        return fileToRead;
    }
    char path[PATH_MAX + 1] = { 0x00 };
    if ((strlen(c) > PATH_MAX) || (realpath(c, path) == NULL)) {
        printf("[SaveFile} get canonnicalize path fail!\n");
        return fileToRead;
    }
    DIR *dirp;
    dirp = opendir(path);

    if (dirp == NULL) {
        return fileToRead;
    }
    for (;;) {
        bool isImage = false;
        struct dirent *direntp = readdir(dirp);
        if (direntp == NULL) {
            break;
        }
        string fileName = direntp->d_name;

        if (!fileName.empty()) {
            string extension = GetExtension(fileName);
            string extensionFlag = "";
            if (find(format.begin(), format.end(), extension) != format.end()) {
                isImage = true;
            }
            if (isImage) {
                string fullpath = dirPath + SLASH + fileName;
                fileToRead.push_back(fullpath);
            }
        }
    }
    closedir(dirp);
    return fileToRead;
}

string FileManager::GetName(const string &filePath)
{
    std::set<char> delims { '/' };
    std::vector<std::string> path = SplitPath(filePath, delims);
    return path.size() < 1 ? "" : path[path.size() - 1];
}

string FileManager::GetParent(const string &filePath)
{
    std::set<char> delims { '/' };
    std::vector<std::string> path = SplitPath(filePath, delims);
    return path.size() < TWO ? "" : path[path.size() - TWO];
}

char *DirName(const char *dirc)
{
    return dirname(const_cast<char *>(dirc));
}

bool FileManager::ChangeDir(const char *argv)
{
    char path[PATH_MAX + 1] = { 0x00 };
    if ((strlen(argv) > PATH_MAX) || (realpath(argv, path) == NULL)) {
        printf("Get canonnicalize path fail!\n");
        return false;
    }
    char *dirc = strdup(argv);
    if (dirc != nullptr) {
        char *dname = DirName(dirc);
        if (chdir(dname) == 0) {
            // do nothing;
        } else {
            free(dirc);
            return false;
        }
    }
    free(dirc);
    return true;
}

std::vector<std::string> FileManager::SplitPath(const std::string &str, const std::set<char> delimiters)
{
    std::vector<std::string> result;
    char const *pch = str.c_str();
    char const *start = pch;
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



