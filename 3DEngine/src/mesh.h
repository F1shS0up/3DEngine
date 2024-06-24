#pragma once
#include "frustum.h"

#include <glm/glm.hpp>

struct vertex
{
	glm::vec3 position;
	glm::vec2 texCoords;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;
	int materialIndex = 0;
};
struct mesh
{
	virtual void Init();
	virtual void Render(class shader* s);
	std::string name;
	bounding_volume* bv;
	vertex* vertices;
	int vertexCount;
	glm::vec4 averagePosition;
	float sqrDistance;
	unsigned int VAO, VBO;
	int materialIndex = 0;
	bool castsShadow = false;
};