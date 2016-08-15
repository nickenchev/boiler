#include <iostream>
#include "vertexdata.h"

VertexData::VertexData(std::vector<glm::vec3> vertices) : vertices(vertices)
{
    owned = false;
}
