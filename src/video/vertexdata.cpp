#include <iostream>
#include "video/vertexdata.h"

using namespace Boiler;

VertexData::VertexData(const std::vector<glm::vec3> &vertices) : vertices(vertices)
{
    owned = false;
}
