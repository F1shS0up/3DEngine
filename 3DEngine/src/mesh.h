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
};
struct mesh
{
	virtual void Init();
	std::string name;
	bounding_volume* bv;

	vertex* vertices;
	int vertexCount;
	unsigned int* indices;
	int indexCount;

	glm::vec4 averagePosition;
	float sqrDistance;
	unsigned int VAO, VBO, EBO;
	int materialIndex = 0;
};