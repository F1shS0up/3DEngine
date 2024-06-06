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
	glm::vec3 ambient = glm::vec3(.1f);
	glm::vec3 diffuse = glm::vec3(.5f);
	class texture2D* diffuseMap = nullptr;
	glm::vec3 specular = glm::vec3(1.f);
	class texture2D* specularMap = nullptr;
	float shininess = 1.0f;
	class texture2D* normalMap = nullptr;
};
struct material_unlit : public material
{
	void Init(class shader* s) override;
	void Set(class shader* s) override;
	glm::vec3 ambient = glm::vec3(1.0f);
	class texture2D* map = nullptr;
};
struct mesh_renderer
{
	mesh* m;
	material* mat;
	class shader* shader;
};

struct mesh_renderer_system : public System
{
	void Init();
	void RenderUsingShader(class shader* s);
	void Render();
};