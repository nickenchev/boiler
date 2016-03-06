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

class Entity : public std::enable_shared_from_this<Entity>
{
    Rect frame;
    std::weak_ptr<Entity> owner;
    unsigned int meshVao;
    unsigned int vertVbo;
    glm::mat4 modelMatrix;
    std::vector<std::shared_ptr<Entity>> children;

    void setOwner(std::shared_ptr<Entity> owner);
    
public:
    Entity();
    Entity(const Rect &frame);
    ~Entity();

    bool flipH, flipV, collides;
    glm::vec3 scale;

    const SpriteSheetFrame *spriteFrame;

    std::vector<std::shared_ptr<Entity>> &getChildren() { return children; }
    void addChild(std::shared_ptr<Entity> child);
    void removeChild(std::shared_ptr<Entity> child);

    Rect &getFrame() { return frame; }
    void setFrame(const Rect &frame);
    inline void refreshFrame();

    unsigned int getVao() const { return meshVao; }
    const glm::mat4 &getMatrix();

protected:
    virtual void onCreate() { }
    
    friend class Engine;
};

#endif /* ENTITY_H */
