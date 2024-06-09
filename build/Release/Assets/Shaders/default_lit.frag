#version 460 core
out vec4 FragColor;

const float PI = 3.14159265359;

struct Material
{
	sampler2D albedoMap;
	vec3 albedo;
	sampler2D normalMap;
	sampler2D metallicMap;
	float metallic;
	sampler2D roughnessMap;
	float roughness;
	sampler2D aoMap;
};

struct Light
{
	vec3 position;
	vec3 color;
	float farPlane;

	bool castShadows;
};

in VS_OUT
{
	vec3 WorldPos;
	vec2 TexCoords;
	mat3 TBN;
	vec4 FragPosLightSpace;
}
fs_in;

uniform vec3 viewPos;
uniform Material material;
uniform Light lights[8];
uniform int lightCount;

layout(binding = 5) uniform sampler2D shadowMap;
layout(binding = 6) uniform samplerCubeArray cubeArray;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return max(F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0), 0.0);
}

float DirectionalShadowCalculation(vec3 normal, Light dirLight)
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
		float bias = max(0.005f * (1.0f - dot(normal, normalize(dirLight.position - fs_in.WorldPos))), 0.005f);

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
	vec3 fragToLight = fs_in.WorldPos - light.position;

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

void main()
{
	// properties
	vec3 N = normalize(texture(material.normalMap, fs_in.TexCoords).rgb * 2.0 - 1.0);
	N = normalize(mat3(fs_in.TBN) * N);
	vec3 V = normalize(viewPos - fs_in.WorldPos);

	vec3 albedo = pow(texture(material.albedoMap, fs_in.TexCoords).rgb * material.albedo, vec3(2.2));
	float metallic = texture(material.metallicMap, fs_in.TexCoords).r * material.metallic;
	float roughness = texture(material.roughnessMap, fs_in.TexCoords).r * material.roughness;
	float ao = texture(material.aoMap, fs_in.TexCoords).r;

	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
	// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
	vec3 F0 = vec3(0.03);
	F0 = mix(F0, albedo, metallic);

	// reflectance equation
	vec3 Lo = vec3(0.0);

	for (int i = 0; i < lightCount; i++)
	{
		// calculate per-light radiance
		vec3 L = normalize(lights[i].position - fs_in.WorldPos);
		vec3 H = normalize(V + L);
		float distance = length(i == 0 ? vec3(1) : (lights[i].position - fs_in.WorldPos));
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lights[i].color * attenuation;

		// Cook-Torrance BRDF
		float NDF = DistributionGGX(N, H, roughness);
		float G = max(GeometrySmith(N, V, L, roughness), 0.000075); // BLACK SPOTS

		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
		vec3 specular = numerator / denominator;

		// kS is equal to Fresnel
		vec3 kS = F;
		// for energy conservation, the diffuse and specular light can't
		// be above 1.0 (unless the surface emits light); to preserve this
		// relationship the diffuse component (kD) should equal 1.0 - kS.
		vec3 kD = vec3(1.0) - kS;
		// multiply kD by the inverse metalness such that only non-metals
		// have diffuse lighting, or a linear blend if partly metal (pure metals
		// have no diffuse light).
		kD *= 1.0 - metallic;

		// scale light by NdotL
		float NdotL = max(dot(N, L), 0.0);
		float shadow = 0.f;
		if (i == 0)
		{
			shadow = DirectionalShadowCalculation(N, lights[0]);
		}
		else
		{
			shadow = PointShadowCalculation(lights[i], N, i - 1);
		}
		// add to outgoing radiance Lo
		Lo += (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadow); // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	}

	vec3 ambient = vec3(0.03) * albedo * ao;

	vec3 color = ambient + Lo;

	// HDR tonemapping
	color = color / (color + vec3(1.0));
	// gamma correct
	color = pow(color, vec3(1.0 / 2.2));

	FragColor = vec4(color, 1.0);
}