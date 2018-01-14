#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "core/logger.h"

class ShaderProgram
{
	Logger logger;
    std::string name;
    unsigned int shaderProgram;

public:
    ShaderProgram(std::string path, std::string name);
    ~ShaderProgram();

    unsigned int getShaderProgram() const { return shaderProgram; }

	bool operator==(const ShaderProgram &rhs) { return getShaderProgram() == rhs.getShaderProgram(); }
	bool operator!=(const ShaderProgram &rhs) { return !(getShaderProgram() == rhs.getShaderProgram()); }
};


#endif /* SHADER_H */
