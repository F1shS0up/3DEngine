#pragma once
#include "glm/glm.hpp"

#include <vector>

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
	std::vector<vertex> vertices;
	unsigned int VAO, VBO;
	mesh() = default;
	mesh(const char* path)
	{
		LoadOBJ(path, vertices);
		InitRenderData();
	}

	bool LoadOBJ(const char* path, std::vector<vertex>& vertices);
	void InitRenderData();
	void Render(class shader* s);
};