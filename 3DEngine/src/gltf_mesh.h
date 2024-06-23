#pragma once
#include "glm/glm.hpp"
#include "mesh.h"

#include <vector>

struct gltf_mesh : public mesh
{
	gltf_mesh() = default;

	std::vector<unsigned int> indices;
	unsigned int EBO;

	void Init() override;
	void Render(class shader* s) override;
};