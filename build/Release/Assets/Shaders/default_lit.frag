#version 460 core
out vec4 FragColor;
struct Material
{
	vec3 ambient;
	vec3 diffuse;
	sampler2D diffuseMap;
	vec3 specular;
	sampler2D specularMap;
	sampler2D normalMap;
	float shininess;
};

struct Light
{
	vec3 positionOrDirection;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float farPlane;

	bool castShadows;
};
in VS_OUT
{
	vec3 WorldPos;
	vec2 TexCoords;
	float invertedUVMultiplier;
	mat3 TBN;
	vec4 FragPosLightSpace;
	int materialIndex;
}
fs_in;

const float constant = 1.0;
const float linear = 0.09;
const float quadratic = 0.032;

uniform vec3 viewPos;
uniform Material material;
uniform Light lights[8];
uniform int lightCount;

layout(binding = 6) uniform sampler2D shadowMap;
layout(binding = 7) uniform samplerCubeArray cubeArray;

float DirectionalShadowCalculation(vec3 normal, vec3 direction)
{
	// Shadow value
	float shadow = 0.0f;
	// Sets lightCoords to cull space
	vec3 lightCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
	if (lightCoords.z <= 1.0f)
	{
		// Get from [-1, 1] range to [0, 1] range just like the shadow map
		lightCoords = (lightCoords + 1.0f) / 2.0f;
		float currentDepth = lightCoords.z;
		// Prevents shadow acne
		float bias = max(0.0005f * (1.0f - dot(normal, direction)), 0.0005f);

		// Smoothens out the shadows
		int sampleRadius = 2;
		vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
		for (int y = -sampleRadius; y <= sampleRadius; y++)
		{
			for (int x = -sampleRadius; x <= sampleRadius; x++)
			{
				float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
				if (currentDepth > closestDepth + bias) shadow += 1.0f;
			}
		}
		// Get average shadow
		shadow /= pow((sampleRadius * 2 + 1), 2);
	}
	return shadow;
}
vec3 gridSamplingDisk[20] = vec3[](vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1), vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1), vec3(1, 1, 0), vec3(1, -1, 0),
								   vec3(-1, -1, 0), vec3(-1, 1, 0), vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1), vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1));
float PointShadowCalculation(Light light, vec3 lightDir, int index)
{
	// get vector between fragment position and light position
	vec3 fragToLight = fs_in.WorldPos - light.positionOrDirection;
	float currentDepth = length(fragToLight);

	float shadow = 0.0;
	float bias = 0;
	int samples = 20;
	float viewDistance = length(viewPos - fs_in.WorldPos);
	float diskRadius = (1.0 + (viewDistance / light.farPlane)) / 25.0;
	for (int i = 0; i < samples; i++)
	{
		float closestDepth = texture(cubeArray, vec4(vec3(fragToLight + gridSamplingDisk[i] * diskRadius), index)).r;
		closestDepth *= light.farPlane; // undo mapping [0;1]
		if (currentDepth - bias > closestDepth) shadow += 1.0;
	}
	shadow /= float(samples);
	return shadow;
}
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = -light.positionOrDirection;
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// combine results
	vec3 ambient = light.ambient * texture(material.diffuseMap, fs_in.TexCoords).rgb * material.ambient;
	vec3 diffuse = light.diffuse * diff * texture(material.diffuseMap, fs_in.TexCoords).rgb * material.diffuse;
	vec3 specular = light.specular * spec * texture(material.specularMap, fs_in.TexCoords).r * material.specular;

	float shadow = DirectionalShadowCalculation(normal, lightDir);
	vec3 lighting = vec3(ambient + (1.0 - shadow) * (diffuse + specular));

	return lighting;
}
vec3 CalcPointLight(Light light, vec3 normal, vec3 viewDir, int index)
{
	vec3 lightDir = normalize(light.positionOrDirection - fs_in.WorldPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// attenuation
	float d = length(light.positionOrDirection - fs_in.WorldPos);
	float attenuation = 1.0 / (constant + linear * d + quadratic * (d * d));
	// combine results
	vec3 ambient = light.ambient * texture(material.diffuseMap, fs_in.TexCoords).rgb * material.ambient;
	vec3 diffuse = light.diffuse * diff * texture(material.diffuseMap, fs_in.TexCoords).rgb * material.diffuse;
	vec3 specular = light.specular * spec * texture(material.specularMap, fs_in.TexCoords).rgb * material.specular;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	float shadow = light.castShadows ? PointShadowCalculation(light, lightDir, index) : 0;
	vec3 lighting = vec3(ambient + (1.0 - shadow) * (diffuse + specular));

	return lighting;
}
void main()
{
	// properties
	vec3 N = normalize(texture(material.normalMap, fs_in.TexCoords).rgb * 2.0 - 1.0);
	N = normalize(mat3(fs_in.TBN) * N);
	vec3 viewDir = normalize(viewPos - fs_in.WorldPos);
	// phase 1: Directional lighting
	vec3 result = vec3(0);
	// phase 2: Point lights
	for (int i = 0; i < lightCount; i++)
	{
		if (i == 0)
			result += CalcDirLight(lights[i], N, viewDir);
		else
			result += CalcPointLight(lights[i], N, viewDir, i - 1);
	}

	FragColor = vec4(result, 1.0);
}