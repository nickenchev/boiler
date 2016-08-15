#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>

#include "core/engine.h"
#include "video/renderer.h"
#include "shaderprogram.h"
#include "opengl.h"

void loadShader(const std::string filename, std::string *output)
{
    SDL_RWops *file = SDL_RWFromFile(filename.c_str(), "r");
    std::string test;
    //SDL_RWops *file = SDL_RWFromFile(filename.c_str(), "r");
    if (file != NULL)
    {
        const int size = SDL_RWsize(file);

        char buffer[size + 1];
        char *buffOffset = buffer;
        int totalRead = 0, charsRead = 1;
        while (totalRead < size && charsRead != 0)
        {
            charsRead = SDL_RWread(file, buffer, (size - totalRead), 1);
            totalRead += charsRead;
            buffOffset += charsRead;
        }
        SDL_RWclose(file);
        buffer[size] = '\0';
        *output = buffer;
    }
    else
    {
        Engine::getInstance().getRenderer().showMessageBox("Error", "Error loading shader program");
    }
}

GLint compileShader(char *src, GLenum shaderType)
{
    GLuint shader;

    //create the shader instance
    shader = glCreateShader(shaderType);

    if (!shader)
    {
        throw std::runtime_error("Error creating the shader object.");
    }
    //pass the shader source for compilation
    glShaderSource(shader, 1, (const char **)&src, NULL);
    //compile the shader
    glCompileShader(shader);

    //check if compilation was successful
    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

    if (!compileStatus)
    {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        char *logBuffer = new char[logLength];
        glGetShaderInfoLog(shader, logLength, NULL, logBuffer);

        std::string compileError(logBuffer);
        Engine::getInstance().getRenderer().showMessageBox("Error", compileError);
        std::cerr << logBuffer << std::endl;
        delete [] logBuffer;

        throw std::runtime_error("Error compiling shader" );
    }

    return shader;
}

ShaderProgram::ShaderProgram(std::string name) : name(name)
{
    //load the shader sources
    std::string vertSrc, fragSrc;

    // get the 
    std::string vertPath = "data/shaders/es3/" + name + ".vert";
    std::string fragPath = "data/shaders/es3/" + name + ".frag";
    loadShader(vertPath, &vertSrc);
    loadShader(fragPath, &fragSrc);

    try
    {
        //create the vertex shader
        GLint vertShader = compileShader((char *)vertSrc.c_str(), GL_VERTEX_SHADER);
        GLint fragShader = compileShader((char *)fragSrc.c_str(), GL_FRAGMENT_SHADER);

        shaderProgram = glCreateProgram();
        if (!shaderProgram)
        {
            throw std::runtime_error("Error creating shader program");
        }

        //attach the loaded shaders to the program
        glAttachShader(shaderProgram, vertShader);
        glAttachShader(shaderProgram, fragShader);

        //bind the named parameters to their vertex attrib indices
        glBindAttribLocation(shaderProgram, 0, "vertCoords");
        glBindAttribLocation(shaderProgram, 1, "texCoords");

        //link the shader program
        glLinkProgram(shaderProgram);
        GLint linkStatus;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
        if (!linkStatus)
        {
            throw std::runtime_error("Error linking the shader program.");
        }

        // detach shaders after a successful link, program keeps them linked
        glDetachShader(shaderProgram, vertShader);
        glDetachShader(shaderProgram, fragShader);
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
    }
    catch (int exception)
    {
        std::cerr << exception;
    }
}

ShaderProgram::~ShaderProgram()
{
    std::cout << "* Deleting Shader Program: " << name << std::endl;
    glDeleteProgram(shaderProgram);
}
