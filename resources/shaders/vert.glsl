#version  330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

// These values are in the view space
out vec3 v_fragPos;
out vec3 v_fragNor;
out vec2 texCoords;
out vec4 fragPosLightSpace;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 lightSpaceMatrix;

void main()
{	
	// Fragment position & normal in world space
	vec3 m_fragPos = vec3(M * vec4(vertPos, 1.0));
	vec3 m_fragNor = vec3(M * vec4(vertNor, 0.0));

	// Fragment position & normal in view space 
	v_fragPos = vec3(V * vec4(m_fragPos, 1.0));
	v_fragNor = vec3(V * vec4(m_fragNor, 0.0));

	texCoords = vertTex;

	fragPosLightSpace = lightSpaceMatrix * vec4(m_fragPos, 1.0);
	
	gl_Position = P * V * M * vec4(vertPos, 1.0);
}
