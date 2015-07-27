#version 410
out vec4 color;
in vec4 fragmentColour;

void main()
{
    color = fragmentColour;
}