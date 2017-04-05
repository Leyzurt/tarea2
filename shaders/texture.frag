#version 330 core
in vec2 Texcoord;
out vec4 outColor;

uniform sampler2D texture1;

void main()
{
	
    outColor = texture(texture1, Texcoord);
	//outColor = vec4(1.0, 1.0, 1.0, 1.0);
};