#ifndef PART_H
#define PART_H

#include <string>
#include "core/logger.h"

namespace Boiler
{

class Engine;

class Part
{
	std::string name;
protected:
	Logger logger;
public:
	Part(const std::string &name) : name(name), logger(name)  { }
    // disable copying
    Part(const Part &part) = delete;
    Part &operator=(const Part &part) = delete;

    virtual ~Part() { }

    virtual void onStart(Engine &engine) = 0;
    virtual void update(double deltaTime) = 0;
};

}

#endif /* PART_H */
