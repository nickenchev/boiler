#version 410

in vec2 fragTexCoords;
out vec4 fragColor;

uniform sampler2D tex2d; 
uniform vec4 entityColor;

void main()
{
	//vec4 sampledColor = vec4(1.0, 1.0, 1.0, texture(tex2d, fragTexCoords.xy).r);
	//fragColor = sampledColor * vec4(0, 0, 1, 1);
	fragColor = vec4(1, 1, 1, 1);
}
