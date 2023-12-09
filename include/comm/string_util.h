//
// Created by just do it on 2023/12/9.
//

#ifndef TINYRPC_STRING_UTIL_H
#define TINYRPC_STRING_UTIL_H
#include <string>
#include <vector>

namespace tinyRPC {

    class StringUtil {
    public:
        static void Trim(std::string& str) {
            str.erase(0, str.find_first_not_of(' '));
            str.erase(str.find_last_not_of(' ') + 1);
        }

        static void Split(const std::string& str, const std::string& d, std::vector<std::string>& res) {
            std::string::size_type start = 0;
            while(true) {
                std::string::size_type pos = str.find(d, start);
                if(pos == std::string::npos) {
                    res.push_back(str.substr(start));
                    return;
                }
                res.push_back(str.substr(start, pos - start));
                start = pos + 1;
            }
        }

        static void ToLower(std::string& str) {
            std::for_each(str.begin(), str.end(), [] (char& c) {
                c = static_cast<char>(std::tolower(c));
            });
        }

        static void ToUpper(std::string& str) {
            std::for_each(str.begin(), str.end(), [] (char& c) {
                c = static_cast<char>(std::toupper(c));
            });
        }
    };

}

#endif //TINYRPC_STRING_UTIL_H
