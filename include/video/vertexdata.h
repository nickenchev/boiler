#ifndef VERTEXDATA_H
#define VERTEXDATA_H

#include <vector>
#include <glm/glm.hpp>

namespace Boiler
{

class VertexData
{
    bool owned;
    std::vector<glm::vec3> vertices;

public:
    VertexData(std::vector<glm::vec3> vertices);

    const float *begin() const { return &(vertices[0].x); }
    int length() const { return vertices.size(); }
    int size() const { return vertices.size() * sizeof(glm::vec3); }
};

}

#endif /* VERTEXDATA_H */
