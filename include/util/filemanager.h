#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace Boiler
{

class FileManager
{
public:
    static std::string readTextFile(const std::string &filePath);
    static std::vector<char> readBinaryFile(const std::string &filePath);

    static std::filesystem::path getDirectory(const std::string &filePath);
    static std::filesystem::path buildPath(const std::filesystem::path &basePath, const std::string &filename);
    static std::string decodeString(const std::string &input);
};

}
