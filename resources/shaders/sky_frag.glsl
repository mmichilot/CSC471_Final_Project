#version 330 core

in vec2 texCoords;

out vec4 color;

uniform sampler2D tex;

void main()
{
    vec4 result = texture(tex, texCoords);
    color = result;
}
