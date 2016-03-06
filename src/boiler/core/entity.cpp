#include <iostream>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include "entity.h"
#include "opengl.h"

Entity::Entity() : Entity(Rect())
{
}

Entity::Entity(const Rect &frame) : scale(1.0f, 1.0f, 1.0f)
{
    if (frame.position.y > 0)
    {
        std::cout << "ASASAS" << std::endl;
    }
    flipH = false;
    flipV = false;
    collides = false;
    this->setFrame(frame);
    this->spriteFrame = nullptr;

    // 2D vertex and texture coords
    GLfloat sizeW = frame.size.getWidth();
    GLfloat sizeH = frame.size.getHeight();

    GLfloat vertices[] =
    {
        0.0f, sizeH,
        sizeW, 0.0f,
        0.0f, 0.0f,

        0.0f, sizeH,
        sizeW, sizeH,
        sizeW, 0.0f
    };

    // setup a VBO for the vertices
    glGenVertexArrays(1, &meshVao);
    glBindVertexArray(meshVao);

    // enable this attrib arrays
    glEnableVertexAttribArray(ATTRIB_ARRAY_VERTEX);
    glEnableVertexAttribArray(ATTRIB_ARRAY_TEXTURE);

    // setup the VBO for verts
    glGenBuffers(1, &vertVbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

    // cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Entity::setOwner(std::shared_ptr<Entity> owner)
{
    this->owner = owner;
    this->refreshFrame();
}

void Entity::addChild(std::shared_ptr<Entity> child)
{
    children.push_back(child);
    child->setOwner(shared_from_this());
    child->onCreate();
    std::cout << child->getFrame().position.x << child->getFrame().position.y << std::endl;
}

void Entity::removeChild(std::shared_ptr<Entity> child)
{
    child->owner.reset();
}

Entity::~Entity()
{
    glDisableVertexAttribArray(ATTRIB_ARRAY_VERTEX);
    glDisableVertexAttribArray(ATTRIB_ARRAY_TEXTURE);

    glDeleteBuffers(1, &vertVbo);
    glDeleteVertexArrays(1, &meshVao);
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
