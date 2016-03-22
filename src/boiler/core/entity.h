#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include "rect.h"

class SpriteSheetFrame;
class SpriteSheet;
class Model;

class Entity : public std::enable_shared_from_this<Entity>
{
    Rect frame;
    std::weak_ptr<Entity> owner;
    glm::mat4 modelMatrix;
    std::vector<std::shared_ptr<Entity>> children;

    void setOwner(std::shared_ptr<Entity> owner);
    
public:
    Entity();
    Entity(const Rect &frame);
    ~Entity();

    bool flipH, flipV, collides;
    glm::vec3 scale;
    glm::vec4 color;
    std::shared_ptr<const Model> model;

    const SpriteSheetFrame *spriteFrame;

    std::vector<std::shared_ptr<Entity>> &getChildren() { return children; }
    void addChild(const std::shared_ptr<Entity> &child);
    void removeChild(const std::shared_ptr<Entity> &child);

    Rect &getFrame() { return frame; }
    void setFrame(const Rect &frame);
    inline void refreshFrame();

    const glm::mat4 &getMatrix();

protected:
    virtual void onCreate() { }
    virtual void update() { }
    
    friend class Engine;
};

#endif /* ENTITY_H */
