#version 410

//in vec2 fragTexCoords;
out vec4 fragColor;

//uniform sampler2D tex2d; 
uniform vec4 entityColor;

void main()
{
	//fragColor = texture(tex2d, fragTexCoords) * entityColor;
	//fragColor = entityColor;
	fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}

