#version 410

in vec4 vertCoords;
//invec2 texCoords;
//out vec2 fragTexCoords;

uniform mat4 MVP;
uniform vec4 entityColor;
//uniform bool usingTexture;

void main()
{
    //fragTexCoords = texCoords;
    
    vec4 vert = vec4(vertCoords.xyz, 1.0);
    gl_Position = MVP * vert;
}
