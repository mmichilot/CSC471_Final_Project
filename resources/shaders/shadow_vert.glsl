#version 330 core

#define MAX_LIGHTS 10

layout (location = 0) in vec3 vertPos;

uniform mat4 M;
uniform mat4 lightSpaceMatrix;

void main()
{
        gl_Position = lightSpaceMatrix * M * vec4(vertPos, 1.0);
}