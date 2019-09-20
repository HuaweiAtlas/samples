/**
 * ============================================================================
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Atlas Sample
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
#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <unordered_map>

inline int parseStrToInt(const std::string &str) { 
    try {
		return std::stoi(str);		
	}
	catch (std::invalid_argument const &e) {
		printf("Bad input: std::invalid_argument thrown\n");
	}
	catch (std::out_of_range const &e) {
		printf("Integer overflow: std::out_of_range thrown\n");
	}
    return 0;
}

class CommandParser{
    public:
        CommandParser() {}
        CommandParser (int &argc, char **argv){
            parseArgs(argc, argv);
        }        

        CommandParser& addOption(const std::string& option, const std::string& defaults="") {
            this->opmap.emplace(option, defaults);
            return *this;
        }

        void parseArgs(int &argc, char **argv) {
            for (int i=1; i < argc; ++i) {
                this->tokens.push_back(std::string(argv[i]));
            }
        }
        
        const std::string& cmdGetOption(const std::string &option) const {
            std::vector<std::string>::const_iterator itr;
            itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
            if (itr != this->tokens.end() && ++itr != this->tokens.end()){
                return *itr;
            }
            if (opmap.count(option)) {
                return opmap.at(option);
            }
            static const std::string empty_string("");
            return empty_string;
        }

        bool cmdOptionExists(const std::string &option) const {
            return std::find(this->tokens.begin(), this->tokens.end(), option)
                   != this->tokens.end();
        }
    private:
        std::vector <std::string> tokens;
        std::unordered_map<std::string, std::string> opmap;
};

#endif /* COMMANDPARSER_H */