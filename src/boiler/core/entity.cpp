#include <iostream>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>

#include "entity.h"
#include "engine.h"
#include "renderer.h"
#include "vertexdata.h"

Entity::Entity() : Entity(Rect())
{
}

Entity::Entity(const Rect &frame) : scale(1.0f, 1.0f, 1.0f)
{
    flipH = false;
    flipV = false;
    collides = false;
    this->setFrame(frame);
    this->spriteFrame = nullptr;

    // 2D vertex and texture coords
    const float sizeW = frame.size.getWidth();
    const float sizeH = frame.size.getHeight();

    glm::vec3 verts[] =
    {
        { 0.0f, sizeH, 0.0f },
        { sizeW, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },

        { 0.0f, sizeH, 0.0f },
        { sizeW, sizeH, 0.0f },
        { sizeW, 0.0f, 0.0f }
        
    };
    VertexData vertData(verts);
    model = Engine::getInstance().getRenderer().loadModel(vertData);
}

void Entity::setOwner(std::shared_ptr<Entity> owner)
{
    this->owner = owner;
    this->refreshFrame();
}

void Entity::addChild(const std::shared_ptr<Entity> &child)
{
    children.push_back(child);
    child->setOwner(shared_from_this());
    child->onCreate();
}

void Entity::removeChild(const std::shared_ptr<Entity> &child)
{
    child->owner.reset();
}

Entity::~Entity()
{
}

const glm::mat4 &Entity::getMatrix()
{
    // offset the player position based on the pivot modifier
    glm::vec3 pivotPos(getFrame().position.x - getFrame().size.getWidth() * getFrame().pivot.x,
                       getFrame().position.y - getFrame().size.getHeight() * getFrame().pivot.y, 0);

    float scaleX = scale.x;
    float scaleY = scale.y;
    if (flipH)
    {
        pivotPos.x += getFrame().size.getWidth();
        scaleX *= -1;
    }
    if (flipV)
    {
        pivotPos.x += getFrame().size.getHeight();
        scaleY *= -1;
    }
    // create the model matrix, by getting a 3D vector from the Entity's vec2 position
    modelMatrix = glm::translate(glm::mat4(1), pivotPos);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(scaleX, scaleY, 1.0f));

    return modelMatrix;
}

inline void Entity::refreshFrame()
{
    if (auto spt = owner.lock())
    {
        this->frame.position += spt->frame.position;
    }
}

void Entity::setFrame(const Rect &frame)
{
    this->frame = frame;
    refreshFrame();

    for (auto &child : children)
    {
        child->refreshFrame();
    }
}
