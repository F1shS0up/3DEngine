#pragma once
#include "ecs/System.hpp"
#include "mesh.h"

struct material
{
	virtual void Init(class shader* s) = 0;
	virtual void Set(class shader* s) = 0;
};
struct material_lit : public material
{
	void Init(class shader* s) override;
	void Set(class shader* s) override;
	class texture2D* albedoMap;
	glm::vec3 albedo = glm::vec3(1.0f);
	class texture2D* normalMap;
	class texture2D* metallicMap;
	float metallic = 1.0f;
	class texture2D* roughnessMap;
	float roughness = 1.0f;
	class texture2D* aoMap;
	float uvMultiplier = 1.0f;
};
struct material_unlit : public material
{
	void Init(class shader* s) override;
	void Set(class shader* s) override;
	glm::vec3 ambient = glm::vec3(1.0f);
	class texture2D* map = nullptr;
	float uvMultiplier = 1.0f;
};
struct mesh_renderer
{
	mesh* m;
	material* mat;
	class shader* shader;
	bool visible = true;
};

struct mesh_renderer_system : public System
{
	void Init();
	void RenderUsingShader(class shader* s);
	void Update();
	void Render();
};