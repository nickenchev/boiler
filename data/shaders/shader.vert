#version 410
in vec2 vPosition;
uniform mat4 MVP;

void main()
{
    vec4 vert = vec4(vPosition.xy, 0.0f, 1.0);
    gl_Position = MVP * vert;
}