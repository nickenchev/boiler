#version 410

in vec2 fragTexCoords;
out vec4 fragColor;

uniform sampler2D tex2d; 
uniform vec4 entityColor;
uniform bool usingTexture;

void main()
{
    vec4 color = entityColor;
    if (usingTexture)
    {
        color = texture(tex2d, fragTexCoords.xy);
    }
    fragColor = color;
}
