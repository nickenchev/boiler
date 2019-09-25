#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>

namespace Boiler
{

class FileManager
{
public:
    static std::string readTextFile(const std::string &filePath);
    static std::vector<char> readBinaryFile(const std::string &filePath);
};

}

#endif /* FILEMANAGER_H */
