#version 410

in vec2 fragTexCoords;
out vec4 fragColor;

uniform sampler2D tex2d; 

void main()
{
    vec4 color = texture(tex2d, fragTexCoords.xy);
    bool fillTransparent = false;
    bool transparent = (color.w == 0);
    if (!transparent || !fillTransparent)
    {
        fragColor = color;
    }
    else
    {
        fragColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);
    }
}
