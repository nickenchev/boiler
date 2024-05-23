#ifndef PART_H
#define PART_H

#include <string>
#include "core/logger.h"

namespace Boiler
{

class Engine;
struct FrameInfo;
class EntityComponentSystem;

class Part
{
protected:
	std::string name;
	Engine &engine;
	Logger logger;
    AssetSet assetSet;

public:
	Part(const std::string &name, Engine &engine) : name(name), engine(engine), logger(name)  { }
    // disable copying
    Part(const Part &part) = delete;
    Part &operator=(const Part &part) = delete;

    virtual ~Part() { }

    virtual void onStart() = 0;
    virtual void update(const FrameInfo &frameInfo) = 0;

    AssetSet &getAssetSet() { return assetSet; }
};

}

#endif /* PART_H */
