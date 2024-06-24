#pragma once
#include "glm/glm.hpp"
#include "mesh.h"

#include <vector>

struct gltf_mesh : public mesh
{
	gltf_mesh() = default;

	unsigned int* indices;
	int indicesCount;
	unsigned int EBO;

	void Init() override;
	void Render(shader* s) override;
};