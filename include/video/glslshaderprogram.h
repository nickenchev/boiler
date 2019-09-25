#ifndef GLSLSHADERPROGRAM_H
#define GLSLSHADERPROGRAM_H

#include <string>
#include "core/logger.h"
#include "video/shaderprogram.h"

namespace Boiler
{

class GLSLShaderProgram : public ShaderProgram
{
	Logger logger;
    unsigned int shaderProgram;

public:
	GLSLShaderProgram(std::string path, std::string vertexShader, std::string fragmentShader);

	void destroy() override;

    unsigned int getShaderProgram() const { return shaderProgram; }

	bool operator==(const GLSLShaderProgram &rhs) { return getShaderProgram() == rhs.getShaderProgram(); }
	bool operator!=(const GLSLShaderProgram &rhs) { return !(getShaderProgram() == rhs.getShaderProgram()); }
};

}

#endif /* GLSLSHADERPROGRAM_H */
