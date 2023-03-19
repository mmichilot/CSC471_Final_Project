#version 330 core

#define DIRECT_LIGHT 0
#define POINT_LIGHT  1
#define SPOT_LIGHT   2

#define MAX_LIGHTS 10

struct Material {
	vec3 diffuse;
	vec3 specular;
	float shininess;

	bool texture_diffuse_enable;
	sampler2D texture_diffuse1;
	bool texture_specular_enable;
	sampler2D texture_specular1;
};

struct Light {
	bool valid;
	int type;
	vec3 position;  // must be in view space
	vec3 direction; 
	float inner_cutoff;
	float outer_cutoff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

in vec3 v_fragPos;
in vec3 v_fragNor;
in vec2 texCoords;
in vec4 fragPosLightSpace;

out vec4 color;

uniform Material material;
uniform Light light[MAX_LIGHTS];
uniform sampler2D shadowMap;

void computeLight(Light light, out vec3 ambient, out vec3 diffuse, out vec3 specular);
void computeAttenuation(Light light, inout vec3 ambient, inout vec3 diffuse, inout vec3 specular);
void computeIntensity(Light light, inout vec3 diffuse, inout vec3 specular);
float shadowCalculation(Light light, vec4 ls_fragPos);

void main()
{
	vec3 result = vec3(0.0f);

	for (int i = 0; i < MAX_LIGHTS; i++) {
		if (light[i].valid) {
			// Compute lighting
			vec3 ambient, diffuse, specular;
			computeLight(light[i], ambient, diffuse, specular);
			
			// Compute attenuation away from a point light or spot light
			if (light[i].type == POINT_LIGHT || light[i].type == SPOT_LIGHT)
				computeAttenuation(light[i], ambient, diffuse, specular);

			// Compute intensity at edges of spotlight
			if (light[i].type == SPOT_LIGHT)
				computeIntensity(light[i], diffuse, specular);
			
			// Compute shadows from light source
			float shadow = shadowCalculation(light[i], fragPosLightSpace);
			result += (ambient + (1.0 - shadow) * (diffuse + specular));
		}
	}
	
	color = vec4(result, 1.0);
}

void computeLight(Light light, out vec3 ambient, out vec3 diffuse, out vec3 specular)
{
	vec3 normal     = normalize(v_fragNor);
	vec3 lightDir   = normalize(light.position - v_fragPos);
	vec3 viewDir    = normalize(-v_fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	
	// Ambient
	if (material.texture_diffuse_enable)
		ambient = light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
	else
		ambient = light.ambient;

	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	if (material.texture_diffuse_enable) 
		diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, texCoords));
	else
		diffuse = light.diffuse * diff * material.diffuse;
	
	// Specular 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	if (material.texture_specular_enable)
		specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoords));
	else
		specular = light.specular * spec * material.specular;
}

void computeAttenuation(Light light, inout vec3 ambient, inout vec3 diffuse, inout vec3 specular)
{
	float distance = length(light.position - v_fragPos);
	float attenuation = 1.0/ (light.constant + light.linear*distance + light.quadratic*(distance*distance));

	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;
}

void computeIntensity(Light light, inout vec3 diffuse, inout vec3 specular)
{
	vec3 lightDir   = normalize(light.position - v_fragPos);
	float theta     = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.inner_cutoff - light.outer_cutoff;
	float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);

	diffuse  *= intensity;
	specular *= intensity;
}

float shadowCalculation(Light light, vec4 ls_fragPos)
{
	vec3 normal     = normalize(v_fragNor);
	vec3 lightDir   = normalize(light.position - v_fragPos);
	
	vec3 projCoords = ls_fragPos.xyz / ls_fragPos.w;
	projCoords = projCoords * 0.5 + 0.5;

	float currentDepth = projCoords.z;

	float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0.00005);
	
	float shadow = 0.0;
	vec2 texelSize = 0.9 / textureSize(shadowMap, 0);
	for (int x = -2; x <= 2; ++x)
	{
		for (int y = -2; y <= 2; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
		}
	}
	
	shadow /= 25.0;
	return shadow;
}