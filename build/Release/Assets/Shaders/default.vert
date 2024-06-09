#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;

out DATA
{
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
	vec4 FragPosLightSpace;

	mat4 model;
}
data_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 directionalLightSpaceMatrix;

void main()
{
	data_out.TexCoords = aTexCoords;
	data_out.FragPos = vec3(model * vec4(aPos, 1.0));
	data_out.Normal = mat3(transpose(inverse(model))) * aNormal;
	data_out.FragPosLightSpace = directionalLightSpaceMatrix * vec4(data_out.FragPos, 1.0);
	data_out.model = model;

	gl_Position = projection * view * vec4(data_out.FragPos, 1.0);
}