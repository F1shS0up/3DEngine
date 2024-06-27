#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
layout(location = 5) in int aMaterialIndex; // Used for batching

out VS_OUT
{
	vec3 WorldPos;
	vec2 TexCoords;
	float invertedUVMultiplier;
	mat3 TBN;
	vec4 FragPosLightSpace;
	int materialIndex;
}
vs_out;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 directionalLightSpaceMatrix;
uniform float uvMultiplier = 1.0;

void main()
{
	vs_out.TexCoords = aTexCoords * uvMultiplier;
	vs_out.invertedUVMultiplier = 1.0 / uvMultiplier;
	vs_out.WorldPos = aPos;
	vec3 T = normalize(aTangent);
	vec3 B = normalize(aBitangent);
	vec3 N = normalize(aNormal);
	vs_out.TBN = mat3(T, B, N);
	vs_out.FragPosLightSpace = directionalLightSpaceMatrix * vec4(vs_out.WorldPos, 1.0);

	vs_out.materialIndex = aMaterialIndex;

	gl_Position = projection * view * vec4(vs_out.WorldPos, 1.0);
}