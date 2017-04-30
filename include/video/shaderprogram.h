#ifndef SHADER_H
#define SHADER_H

#include <string>

class ShaderProgram
{
    std::string name;
    unsigned int shaderProgram;

public:
    ShaderProgram(std::string path, std::string name);
    ~ShaderProgram();

    unsigned int getShaderProgram() const { return shaderProgram; }
};


#endif /* SHADER_H */
