#pragma once
#include "ecs/System.hpp"
#include "material.h"
#include "model.h"

#include <string>
#include <unordered_map>
#include <vector>

struct mesh_renderer
{
	model* m;
	std::vector<material*> materials;
	std::unordered_map<std::string, bool> visibleMeshes;
	float sqrDistance = 0.0f; // Used for sorting
};

struct mesh_renderer_system : public System
{
	std::vector<Entity> orderedRenders;
	void Init();
	void RenderShadowMap(class shader* s);
	void Update();
	void Render(bool transparentPass);
};