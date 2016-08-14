#ifndef PART_H
#define PART_H

#include <vector>
#include <memory>
#include "component.h"
#include "entity.h"

class Engine;

class Part : public Component, public Entity
{
public:
    Part() : Component("Part"), Entity(Rect(0, 0, 0, 0)) { }

    // disable copying
    Part(const Part &part) = delete;
    Part &operator=(const Part &part) = delete;

    virtual void start() = 0;
    virtual void handleInput() = 0;
    virtual void update(const float delta) = 0;
};

#endif /* PART_H */
