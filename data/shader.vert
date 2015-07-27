#version 410
in vec3 vPosition;
uniform mat4 MVP;

layout(location = 1) in vec3 vertexColour;

out vec4 fragmentColour;

void main()
{
    vec4 vert = vec4(vPosition, 1.0);
    gl_Position = MVP * vert;

    //colour related
    fragmentColour = vec4(vertexColour, 1.0);
}