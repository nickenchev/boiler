#ifndef OPENGLMODEL_H
#define OPENGLMODEL_H

#include "model.h"
#include "opengl.h"
#include "vertexdata.h"

namespace Boiler
{

enum class AttribArray
{
	Vertex = 0,
	Texture = 1
};

class OpenGLModel : public Model
{
    unsigned int meshVao;
    unsigned int vertVbo;

public:
    OpenGLModel(const VertexData &data);
    virtual ~OpenGLModel();

    unsigned int getVao() const { return meshVao; }
};

}

#endif /* OPENGLMODEL_H */
