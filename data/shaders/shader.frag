#version 410

in vec2 fragTexCoords;
out vec4 fragColor;

uniform sampler2D tex2d; 
uniform vec4 entityColor;

void main()
{
    vec4 color = entityColor;
    color += texture(tex2d, fragTexCoords.xy);
    fragColor = color;
}
