#ifndef ENTITY_H
#define ENTITY_H

#include "rect.h"
#include <glm/glm.hpp>

#define ATTRIB_ARRAY_VERTEX 0
#define ATTRIB_ARRAY_TEXTURE 1

class SpriteSheetFrame;
class SpriteSheet;

class Entity
{
    unsigned int meshVao;
    unsigned int vertVbo;
    glm::mat4 modelMatrix;
    
public:
    Entity();
    Entity(Rect frame);
    ~Entity();

    Rect frame;
    glm::vec2 pivot;
    glm::vec2 scale;

    const SpriteSheet *spriteSheet;
    const SpriteSheetFrame *spriteFrame;

    unsigned int getVao() const { return meshVao; }
    const glm::mat4 &getMatrix();
};


#endif /* ENTITY_H */
