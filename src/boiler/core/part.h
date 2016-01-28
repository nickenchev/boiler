#ifndef PART_H
#define PART_H

#include <vector>
#include <memory>
#include "component.h"

class Engine;
class Entity;

class Part : public Component
{
protected:
    std::vector<std::shared_ptr<Entity>> entities;

public:
    Part(Engine &engine) : Component("Part", engine) { }

    // disable copying
    Part(const Part &part) = delete;
    Part &operator=(const Part &part) = delete;

    virtual void start() = 0;
    virtual void handleInput() = 0;
    virtual void update(const float delta) = 0;

    void addEntity(std::shared_ptr<Entity> entity);
    const std::vector<std::shared_ptr<Entity>> &getEntities() const { return entities; }
};

#endif /* PART_H */
