#include "video/openglmodel.h"

OpenGLModel::OpenGLModel(const VertexData &data) : Model(data.length())
{
    // setup a VBO for the vertices
    glGenVertexArrays(1, &meshVao);
    glBindVertexArray(meshVao);

    // enable this attrib arrays
    glEnableVertexAttribArray(ATTRIB_ARRAY_VERTEX);
    glEnableVertexAttribArray(ATTRIB_ARRAY_TEXTURE);

    // setup the VBO for verts
    glGenBuffers(1, &vertVbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertVbo);

    // TODO: Handle GL_STATIC_DRAW properly, might be dynamic
    glBufferData(GL_ARRAY_BUFFER, data.size(), data.begin(), GL_STATIC_DRAW);
    glVertexAttribPointer(ATTRIB_ARRAY_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

    // cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

OpenGLModel::~OpenGLModel()
{
	if (meshVao)
	{
		glBindVertexArray(meshVao);
		glDisableVertexAttribArray(ATTRIB_ARRAY_VERTEX);
		glDisableVertexAttribArray(ATTRIB_ARRAY_TEXTURE);
		glBindVertexArray(0);
		glDeleteBuffers(1, &vertVbo);
		glDeleteVertexArrays(1, &meshVao);
	}
}
