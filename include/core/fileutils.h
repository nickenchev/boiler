#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>

namespace Boiler
{

class FileUtils
{
public:
    static std::string getDirectory(const std::string fullPath);
    static std::string buildPath(const std::string directory, const std::string filename);
};

}

#endif /* FILEUTILS_H */
