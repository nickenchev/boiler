#include "part.h"

void Part::addEntity(std::shared_ptr<Entity> entity)
{
    entities.push_back(std::move(entity));
}
