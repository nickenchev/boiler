#ifndef RENDERDETAILS_H
#define RENDERDETAILS_H

struct RenderDetails
{
	const ShaderProgram *shaderProgram;
    unsigned int mvpUniform;
    unsigned int colorUniform;
    bool usingTexUniform;
    glm::mat4 viewProjection;
    glm::mat4 camViewProjection;

    RenderDetails()
    {
        mvpUniform = -1;
        colorUniform = -1;
        usingTexUniform = -1;
    }
};

#endif /* RENDERDETAILS_H */
