#include "video/openglmodel.h"

using namespace Boiler;

OpenGLModel::OpenGLModel(const VertexData &data) : Model(data.vertexArray().size())
{
    // setup a VBO for the vertices
    glGenVertexArrays(1, &meshVao);
    glBindVertexArray(meshVao);

    // enable this attrib arrays
    glEnableVertexAttribArray((GLuint)AttribArray::Vertex);
    //glEnableVertexAttribArray((GLuint)AttribArray::Texture);

    // setup the VBO for verts
    glGenBuffers(1, &vertVbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertVbo);

    // TODO: Handle GL_STATIC_DRAW properly, might be dynamic
    glBufferData(GL_ARRAY_BUFFER, data.vertexSize(), data.vertexBegin(), GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)AttribArray::Vertex, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

    // cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

OpenGLModel::~OpenGLModel()
{
	if (meshVao)
	{
		glBindVertexArray(meshVao);
		glDisableVertexAttribArray((GLuint)AttribArray::Vertex);
		glDisableVertexAttribArray((GLuint)AttribArray::Texture);
		glBindVertexArray(0);
		glDeleteBuffers(1, &vertVbo);
		glDeleteVertexArrays(1, &meshVao);
	}
}
