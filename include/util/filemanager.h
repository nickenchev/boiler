#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>

namespace Boiler
{

class FileManager
{
public:
    static std::string readTextFile(const std::string &filePath);
};

}

#endif /* FILEMANAGER_H */
