#ifndef ENTITY_H
#define ENTITY_H

#include "rect.h"
#include <GLM/glm.hpp>

#define ATTRIB_ARRAY_VERTEX 0
#define ATTRIB_ARRAY_TEXTURE 1

class Entity
{
    unsigned int meshVao;
    unsigned int vertVbo;
    
public:
    Entity();
    Entity(Rect frame);
    ~Entity();

    Rect frame;
    glm::vec2 scale;

    unsigned int getVao() const { return meshVao; }
};


#endif /* ENTITY_H */
