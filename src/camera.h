#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <memory>
#include "rect.h"

class Entity;

class Camera

{
    std::shared_ptr<Entity> centralEntity;

public:
    Camera(const Rect &frame);

    Rect frame;

    void setCentralEntity(std::shared_ptr<Entity> centralEntity);
    const std::shared_ptr<Entity> &getCentralEntity() const { return centralEntity; }
};


#endif /* CAMERA_H */
