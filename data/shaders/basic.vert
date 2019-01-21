#version 410

in vec4 vertCoords;
in vec2 texCoords;
out vec2 fragTexCoords;

uniform mat4 MVP;
uniform mat4 entityColor;
uniform bool usingTexture;

void main()
{
    fragTexCoords = vertCoords.zw;
    
    vec4 vert = vec4(vertCoords.xy, 0.0f, 1.0);
    gl_Position = MVP * vert;
}
