#ifndef PART_H
#define PART_H

#include <string>
#include "core/logger.h"

class Part
{
	std::string name;
protected:
	Logger logger;
public:
	Part(const std::string &name) : logger(name), name(name) { }
    // disable copying
    Part(const Part &part) = delete;
    Part &operator=(const Part &part) = delete;

    virtual void onStart() = 0;
    virtual void update(double deltaTime) { }
};

#endif /* PART_H */
