#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera/platformercamera.h"
#include "core/entity.h"

void PlatformerCamera::update(glm::vec3 moveAmount)
{
    if (centralEntity)
    {
        const Rect &player = centralEntity->getFrame();

        // configure the cam deadzone
        float camDeadSizeHalf = 25.0f;
        float camDeadXMin = frame.size.getWidth() / 2 - camDeadSizeHalf;
        float camDeadXMax = frame.size.getWidth() / 2 + camDeadSizeHalf;
        float camDeadYMin = frame.size.getHeight() / 2 - camDeadSizeHalf;
        float camDeadYMax = frame.size.getHeight() / 2 + camDeadSizeHalf;

        // move the camera rect
        float relativeX = player.position.x - frame.position.x;
        float relativeY = player.position.y - frame.position.y;

        if ((relativeX <= camDeadXMin && moveAmount.x < 0) ||
            (relativeX >= camDeadXMax && moveAmount.x > 0))
        {
            frame.position.x += moveAmount.x;
        }
        
        if ((relativeY <= camDeadYMin && moveAmount.y < 0) ||
            (relativeY >= camDeadYMax && moveAmount.y > 0))
        {
            frame.position.y += moveAmount.y;
        }

        // ensure camera isn't outside of map bounds
        if (frame.position.x < 0) frame.position.x = 0;
        else if (frame.getMax(DIM_X) > mapSize.getWidth()) frame.position.x = frame.getMax(DIM_X) - frame.size.getWidth();

        if (frame.position.y < 0) frame.position.y = 0;
        else if (frame.getMax(DIM_Y) > mapSize.getHeight()) frame.position.y = frame.getMax(DIM_Y) - frame.size.getHeight();
    }
}

glm::mat4 PlatformerCamera::getViewMatrix() const
{
    glm::mat4 view = glm::lookAt(glm::vec3(frame.position.x, frame.position.y, 1.0f),
                                    glm::vec3(frame.position.x, frame.position.y, -1.0f),
                                    glm::vec3(0, 1.0f, 0));

    return view;
}
