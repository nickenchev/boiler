#ifndef SHADER_H
#define SHADER_H

#include <string>

typedef unsigned int GLuint;

class ShaderProgram
{
    std::string name;
    GLuint shaderProgram;

public:
    ShaderProgram(std::string name);
    ~ShaderProgram();

    GLuint getShaderProgram() const { return shaderProgram; }
};


#endif /* SHADER_H */
