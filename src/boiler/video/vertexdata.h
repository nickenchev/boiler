#ifndef VERTEXDATA_H
#define VERTEXDATA_H

#include <glm/glm.hpp>
#include <GSL/gsl.h>

class VertexData
{
    bool owned;
    gsl::span<glm::vec3> vertices;

public:
    VertexData(gsl::span<glm::vec3> array);
    virtual ~VertexData();

    float *begin() const { return &(vertices[0].x); }
    int length() const { return vertices.size(); }
    int size() const { return vertices.size() * sizeof(glm::vec3); }
};


#endif /* VERTEXDATA_H */
