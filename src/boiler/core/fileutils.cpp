#include "core/fileutils.h"

std::string FileUtils::getDirectory(const std::string fullPath)
{
    std::string directory;

    // try to find the index of the last slash
    const size_t lastSlashIdx = fullPath.rfind('/');
    if (lastSlashIdx != std::string::npos)
    {
        directory = fullPath.substr(0, lastSlashIdx);
    }
    return directory;
}

std::string FileUtils::buildPath(const std::string directory, const std::string filename)
{
    return directory + '/' + filename;
}
