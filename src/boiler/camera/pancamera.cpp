#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "pancamera.h"

void PanCemera::update(glm::vec3 moveAmount)
{
    frame.position += moveAmount;

    // ensure camera isn't outside of map bounds
    if (frame.position.x < 0) frame.position.x = 0;
    else if (frame.getMax(DIM_X) > mapSize.getWidth()) frame.position.x = frame.getMax(DIM_X) - frame.size.getWidth();

    if (frame.position.y < 0) frame.position.y = 0;
    else if (frame.getMax(DIM_Y) > mapSize.getHeight()) frame.position.y = frame.getMax(DIM_Y) - frame.size.getHeight();
}

glm::mat4 PanCemera::getViewMatrix() const
{
    // TODO: Handle 3D version
    glm::mat4 view = glm::lookAt(glm::vec3(frame.position.x, frame.position.y, 1.0f),
                                    glm::vec3(frame.position.x, frame.position.y, -1.0f),
                                    glm::vec3(0, 1.0f, 0));

    return view;
}
