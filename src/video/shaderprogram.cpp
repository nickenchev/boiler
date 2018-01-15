#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>

#include "core/boiler.h"
#include "video/renderer.h"
#include "video/shaderprogram.h"
#include "video/opengl.h"
#include "util/filemanager.h"

GLint compileShader(const char *src, GLenum shaderType)
{
    //create the shader instance
    GLuint shader = glCreateShader(shaderType);

    if (!shader)
    {
        throw std::runtime_error("Error creating the shader object.");
    }

    //compile the shader, and make sure all is good
    glShaderSource(shader, 1, (const char **)&src, NULL);
    glCompileShader(shader);
    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

    if (!compileStatus)
    {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        char *logBuffer = new char[logLength];
        glGetShaderInfoLog(shader, logLength, NULL, logBuffer);

        std::string compileError(logBuffer);
        delete [] logBuffer;
		
		std::cerr << "Compilation Error: " << compileError << std::endl;
        throw std::runtime_error("Error compiling shader" );
    }

    return shader;
}

ShaderProgram::ShaderProgram(std::string path, std::string name) : name(name), logger("Shader Program " + name)
{
    std::string vertPath = path + name + ".vert";
    std::string fragPath = path + name + ".frag";
    std::string vertSrc = FileManager::readTextFile(vertPath);
    std::string fragSrc = FileManager::readTextFile(fragPath);

	logger.log("Vertex: " + vertPath);
	logger.log("Fragment: " + fragPath);

	//create the vertex shader
	GLint vertShader = compileShader(vertSrc.c_str(), GL_VERTEX_SHADER);
	GLint fragShader = compileShader(fragSrc.c_str(), GL_FRAGMENT_SHADER);

	shaderProgram = glCreateProgram();
	if (!shaderProgram)
	{
		throw std::runtime_error("Error creating shader program");
	}

	//attach the loaded shaders to the program
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);
	if (glGetError() != GL_NO_ERROR)
	{
		logger.error("Error attaching shaders to program.");
	}

	//bind the named parameters to their vertex attrib indices
	glBindAttribLocation(shaderProgram, 0, "vertCoords");
	glBindAttribLocation(shaderProgram, 1, "texCoords");
	if (glGetError() != GL_NO_ERROR)
	{
		logger.error("Couldn't bind shaders to program.");
	}

	//link the shader program
	glLinkProgram(shaderProgram);
	GLint linkStatus;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
	if (!linkStatus)
	{
		GLint logLength;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
		
		logger.error(SDL_GetError());
		throw std::runtime_error("Error linking the shader program.");
	}

	// detach shaders after a successful link, program keeps them linked
	glDetachShader(shaderProgram, vertShader);
	glDetachShader(shaderProgram, fragShader);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
}

ShaderProgram::~ShaderProgram()
{
	logger.log("Cleaning Up");
    glDeleteProgram(shaderProgram);
}
