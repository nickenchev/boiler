#ifndef SPVSHADERPROGRAM_H
#define SPVSHADERPROGRAM_H

#include <string>
#include "shaderprogram.h"
#include "core/logger.h"

namespace Boiler
{

class SPVShaderProgram : public ShaderProgram
{
	Logger logger;
public:
	SPVShaderProgram(std::string path, std::string vertexShader, std::string fragmentShader);
    ~SPVShaderProgram();
};

}

#endif /* SPVSHADERPROGRAM_H */
