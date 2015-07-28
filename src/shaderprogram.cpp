#include <iostream>
#include <fstream>
#include "shaderprogram.h"
#include "opengl.h"

void loadShader(const std::string filename, std::string *output)
{
    //open the file
    std::ifstream file(filename);

    if (!file.is_open())
    {
        throw std::runtime_error("Error openning the file.");
    }

    std::string line;
    while (getline(file, line))
    {
        output->append(line + "\n");
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

        std::cerr << logBuffer << std::endl;
        delete logBuffer;

        throw std::runtime_error("Error compiling shader" );
    }

    return shader;
}

ShaderProgram::ShaderProgram(std::string name) : name(name)
{
    //load the shader sources
    std::string vertSrc, fragSrc;
    loadShader("data/shaders/" + name + ".vert", &vertSrc);
    loadShader("data/shaders/" + name + ".frag", &fragSrc);

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
    std::cout << "* Deleting shader: " << name << std::endl;
    glDeleteProgram(shaderProgram);
}
