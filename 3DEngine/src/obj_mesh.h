#pragma once
#include "glm/glm.hpp"
#include "mesh.h"

#include <vector>

struct obj_mesh : public mesh
{
	obj_mesh() = default;

	void Init() override;
	void Render(class shader* s) override;
};