#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include "rect.h"

typedef unsigned long EntityId;

class SpriteSheetFrame;
class SpriteSheet;
class Model;

class Entity : public std::enable_shared_from_this<Entity>
{
	EntityId id;
    Rect frame;
    std::weak_ptr<Entity> owner;
    glm::mat4 modelMatrix;
    std::vector<std::shared_ptr<Entity>> children;

    void setOwner(std::shared_ptr<Entity> owner);
    
public:
    Entity();
	Entity(EntityId id) : id{id} { }
    Entity(const Rect &frame);
    ~Entity();

	bool operator==(const Entity &entity) const { return entity.getId() == this->getId(); }

	EntityId getId() const { return id; }

    bool flipH, flipV, collides;
    glm::vec3 scale;
    glm::vec4 color;
    std::shared_ptr<const Model> model;
    bool absolute;

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
    
    friend class Boiler;
};

#endif /* ENTITY_H */
