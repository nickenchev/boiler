#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>

class FileManager
{
public:
    static std::string readTextFile(const std::string &filePath);
};


#endif /* FILEMANAGER_H */
