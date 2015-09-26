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
        }
    }
}
