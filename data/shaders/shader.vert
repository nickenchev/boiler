#version 410
in vec4 vPosition;
out vec2 texCoords;
uniform mat4 MVP;

void main()
{
    texCoords = vPosition.zw;
    vec4 vert = vec4(vPosition.xy, 0.0f, 1.0);
    gl_Position = MVP * vert;
}
