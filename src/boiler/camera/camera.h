#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <memory>
#include "core/rect.h"

class Entity;

class Camera
{
protected:
    std::shared_ptr<Entity> centralEntity;

public:
    explicit Camera(const Rect &frame, const Size &mapSize);

    virtual void update(glm::vec3 moveAmount) = 0;
    virtual glm::mat4 getViewMatrix() const = 0;

    Rect frame;
    Size mapSize;

    void setCentralEntity(std::shared_ptr<Entity> centralEntity);
    const std::shared_ptr<Entity> &getCentralEntity() const { return centralEntity; }
};


#endif /* CAMERA_H */
