#pragma once
#include "ecs/System.hpp"
#include "material.h"
#include "model.h"

#include <string>
#include <unordered_map>
#include <vector>

enum render_type
{
	DYNAMIC,
	STATIC
};
struct mesh_renderer
{
	model* m;
	std::vector<material*> materials;
	render_type type;

	float sqrDistance = 0.0f; // Used for sorting
	std::unordered_map<int, bool> visibleMeshes;
};

struct mesh_renderer_system : public System
{
	std::vector<Entity> orderedRenders;
	void OnEntityAdded(Entity entity) override;
	void RenderShadowMap(class shader* s);
	void Update();
	void Render(bool transparentPass);
};