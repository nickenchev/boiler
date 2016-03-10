#include <iostream>
#include "vertexdata.h"

VertexData::VertexData(gsl::span<glm::vec3> array)
{
    owned = false;
    this->vertices = array;
}

VertexData::~VertexData()
{
}
