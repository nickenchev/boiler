#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "core/logger.h"

namespace Boiler
{

class ShaderProgram
{
	Logger logger;
    unsigned int shaderProgram;

public:
	ShaderProgram(std::string path, std::string vertexShader, std::string fragmentShader);
    ~ShaderProgram();

    unsigned int getShaderProgram() const { return shaderProgram; }

	bool operator==(const ShaderProgram &rhs) { return getShaderProgram() == rhs.getShaderProgram(); }
	bool operator!=(const ShaderProgram &rhs) { return !(getShaderProgram() == rhs.getShaderProgram()); }
};

}

#endif /* SHADER_H */
