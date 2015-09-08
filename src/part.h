#ifndef PART_H
#define PART_H

#include <vector>
#include <memory>

class Engine;
class Entity;

class Part
{
protected:
    std::vector<std::shared_ptr<Entity>> entities;
    Engine *engine;

public:
    Part(Engine *engine) : engine(engine) { }

    virtual void start() = 0;
    virtual void handleInput() = 0;
    virtual void update(const float delta) = 0;

    const Engine *getEngine() const { return engine; }
    void addEntity(std::shared_ptr<Entity> entity);
    const std::vector<std::shared_ptr<Entity>> &getEntities() const { return entities; }
};

#endif /* PART_H */
