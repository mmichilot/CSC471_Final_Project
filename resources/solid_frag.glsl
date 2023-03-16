#version 330 core 
in vec3 fragNor;
out vec4 color;

uniform vec4 solidColor;

void main()
{
	color = solidColor;
}
