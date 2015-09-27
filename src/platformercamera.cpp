#include <iostream>
#include "platformercamera.h"
#include "entity.h"

void PlatformerCamera::update(glm::vec2 moveAmount)
{
    if (centralEntity)
    {
        if (centralEntity->frame.getMax(DIM_X))
        {
            centralEntity->frame.position += moveAmount;
            frame.position.x = centralEntity->frame.position.x - (frame.size.getWidth() / 2);
            frame.position.y = centralEntity->frame.position.y - (frame.size.getHeight() / 2);
        }
    }
}
