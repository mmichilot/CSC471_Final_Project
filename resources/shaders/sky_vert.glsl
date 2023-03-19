#version  330 core
layout(location = 0) in vec3 vertPos;
layout(location = 2) in vec2 vertTex;

out vec2 texCoords;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

void main()
{	
	texCoords = vertTex;
	
	gl_Position = P * V * M * vec4(vertPos, 1.0);
}