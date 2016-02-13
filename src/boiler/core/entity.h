#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include "rect.h"

#define ATTRIB_ARRAY_VERTEX 0
#define ATTRIB_ARRAY_TEXTURE 1

class SpriteSheetFrame;
class SpriteSheet;

class Entity
{
    unsigned int meshVao;
    unsigned int vertVbo;
    glm::mat4 modelMatrix;
    std::vector<std::shared_ptr<Entity>> children;
    
public:
    Entity();
    Entity(Rect frame);
    ~Entity();

    Rect frame;
    bool flipH, flipV, collides;
    glm::vec3 scale;

    std::shared_ptr<SpriteSheet> spriteSheet;
    const SpriteSheetFrame *spriteFrame;

    std::vector<std::shared_ptr<Entity>> &getChildren() { return children; }
    void addChild(std::shared_ptr<Entity> child);

    unsigned int getVao() const { return meshVao; }
    const glm::mat4 &getMatrix(const glm::vec3 &offset);
    const glm::mat4 &getMatrix();

protected:
    virtual void onCreate() { }
};

#endif /* ENTITY_H */
