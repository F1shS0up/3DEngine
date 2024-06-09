#version 460 core
out vec4 FragColor;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	sampler2D diffuseMap;
	vec3 specular;
	sampler2D specularMap;
	float shininess;
	sampler2D normalMap;
};

struct PointLight
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

	float farPlane;

	bool castShadows;
};
struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	bool castShadows;
};

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;
in vec4 FragPosLightSpace;

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[8];
uniform int pointLightCount;

layout(binding = 3) uniform sampler2D shadowMap;
layout(binding = 4) uniform samplerCubeArray cubeArray;

float DirectionalShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal)
{
	// Shadow value
	float shadow = 0.0f;
	// Sets lightCoords to cull space
	vec3 lightCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	if (lightCoords.z <= 1.0f)
	{
		// Get from [-1, 1] range to [0, 1] range just like the shadow map
		lightCoords = (lightCoords + 1.0f) / 2.0f;
		float currentDepth = lightCoords.z;
		// Prevents shadow acne
		float bias = max(0.0005f * (1.0f - dot(normal, dirLight.direction)), 0.0005f);

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
float PointShadowCalculation(vec4 fragPosLightSpace, PointLight light, vec3 lightDir, int index)
{
	// get vector between fragment position and light position
	vec3 fragToLight = FragPos - light.position;

	float currentDepth = length(fragToLight);

	float shadow = 0.0;
	float bias = 0.15;
	int samples = 20;
	float viewDistance = length(viewPos - FragPos);
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

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// combine results
	vec3 ambient = light.ambient * texture(material.diffuseMap, TexCoords).rgb * material.diffuse;
	vec3 diffuse = light.diffuse * diff * texture(material.diffuseMap, TexCoords).rgb * material.diffuse;
	vec3 specular = light.specular * spec * texture(material.specularMap, TexCoords).r * material.specular;

	float shadow = DirectionalShadowCalculation(FragPosLightSpace, lightDir, normal);
	vec3 lighting = vec3(ambient + (1.0 - shadow) * (diffuse + specular));

	return lighting;
}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, int index)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// combine results
	vec3 ambient = light.ambient * texture(material.diffuseMap, TexCoords).rgb * material.diffuse;
	vec3 diffuse = light.diffuse * diff * texture(material.diffuseMap, TexCoords).rgb * material.diffuse;
	vec3 specular = light.specular * spec * texture(material.specularMap, TexCoords).rgb * material.specular;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	float shadow = light.castShadows ? PointShadowCalculation(FragPosLightSpace, light, lightDir, index) : 0;
	vec3 lighting = vec3(ambient + (1.0 - shadow) * (diffuse + specular));

	return lighting;
}

void main()
{
	vec3 tangentViewPos = viewPos;
	vec3 tangentFragPos = FragPos;
	// properties
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(tangentViewPos - tangentFragPos);

	// phase 1: Directional lighting
	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	// phase 2: Point lights
	for (int i = 0; i < pointLightCount; i++)
	{
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, i);
	}

	FragColor = vec4(result, 1.0);
}