#ifndef VERTEXDATA_H
#define VERTEXDATA_H

#include <vector>
#include "core/math.h"

namespace Boiler
{

class VertexData
{
    bool owned;
    std::vector<vec3> vertices;

public:
    VertexData(const std::vector<vec3> &vertices);

    const float *begin() const { return &(vertices[0].x); }
    int length() const { return vertices.size(); }
    int size() const { return vertices.size() * sizeof(vec3); }
};

}

#endif /* VERTEXDATA_H */
