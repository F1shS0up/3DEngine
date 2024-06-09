#version 330 core
in vec4 WorldPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
	// get distance between fragment and light source
	float lightDistance = length(WorldPos.xyz - lightPos);

	// map to [0;1] range by dividing by farPlane
	lightDistance = lightDistance / farPlane;

	// write this as modified depth
	gl_FragDepth = lightDistance;
}