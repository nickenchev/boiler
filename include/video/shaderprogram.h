#ifndef SHADER_H
#define SHADER_H

namespace Boiler
{

class ShaderProgram
{
public:
	virtual ~ShaderProgram() { }
	virtual void destroy() = 0;
};

}

#endif /* SHADER_H */
