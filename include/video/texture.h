#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

namespace Boiler
{

class Texture
{
    std::string filePath;

protected:
    Texture(const std::string filePath) : filePath(filePath) { }

public:
    const std::string &getFilePath() const { return filePath; }
};

}

#endif /* TEXTURE_H */
