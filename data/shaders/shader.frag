#version 410

in vec2 fragTexCoords;
out vec4 fragColor;

uniform sampler2D tex2d; 

void main()
{
    fragColor = texture(tex2d, fragTexCoords);
    //fragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
}
