#ifndef TMXLOADER_H
#define TMXLOADER_H

#include <memory>
#include <string>
#include "tmx.h"

class TmxLoader
{
public:
    static std::unique_ptr<ensoft::Map> loadMap(std::string filename);
};


#endif /* TMXLOADER_H */
