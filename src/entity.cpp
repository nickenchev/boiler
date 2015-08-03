#include <iostream>
#include "entity.h"
#include "opengl.h"

Entity::Entity() : Entity(Rect())
{
}

Entity::Entity(Rect frame)
{
    this->frame = frame;

    // 2D vertex and texture coords
    GLfloat sizeW = frame.size.width;
    GLfloat sizeH = frame.size.height;

    GLfloat vertices[] =
    {
        0.0f, sizeH,
        sizeW, 0.0f,
        0.0f, 0.0f,

        0.0f, sizeH,
        sizeW, sizeH,
        sizeW, 0.0f
    };

    // setup a VBO for the vertices
    glGenVertexArrays(1, &meshVao);
    glBindVertexArray(meshVao);

    // enable this attrib arrays
    glEnableVertexAttribArray(ATTRIB_ARRAY_VERTEX);
    glEnableVertexAttribArray(ATTRIB_ARRAY_TEXTURE);

    // setup the VBO for verts
    glGenBuffers(1, &vertVbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

    // cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Entity::~Entity()
{
    glDisableVertexAttribArray(ATTRIB_ARRAY_VERTEX);
    glDisableVertexAttribArray(ATTRIB_ARRAY_TEXTURE);

    glDeleteBuffers(1, &vertVbo);
    glDeleteVertexArrays(1, &meshVao);
}
