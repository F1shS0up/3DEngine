#include "mesh_renderer.h"

#include "ecs/coordinator.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "resource_manager.h"
#include "shader.h"
#include "texture.h"
#include "transform.h"

#include <iostream>

extern Coordinator gCoordinator;

void material_lit::Init(shader* s)
{
	s->SetInteger("material.diffuseMap", 0, true);
	s->SetInteger("material.specularMap", 1, true);
	s->SetInteger("material.normalMap", 2, true);
	s->SetInteger("shadowMap", 3, true);
	s->SetInteger("cubeArray", 4, true);
	if (diffuseMap == nullptr)
	{
		diffuseMap = resource_manager::GetTexture("DEFAULT");
	}
	if (specularMap == nullptr)
	{
		specularMap = resource_manager::GetTexture("DEFAULT");
	}
	if (normalMap == nullptr)
	{
		normalMap = resource_manager::GetTexture("DEFAULT_NORMAL_MAP");
	}
}

void material_lit::Set(shader* s)
{
	s->SetVector3f("material.ambient", ambient, true);
	s->SetVector3f("material.diffuse", diffuse, true);
	s->SetVector3f("material.specular", specular, true);
	s->SetFloat("material.shininess", shininess, true);
	glActiveTexture(GL_TEXTURE0);
	diffuseMap->Bind();
	glActiveTexture(GL_TEXTURE1);
	specularMap->Bind();
	glActiveTexture(GL_TEXTURE2);
	normalMap->Bind();
}

void material_unlit::Init(shader* s)
{
	s->SetInteger("material.map", 0, true);
}

void material_unlit::Set(shader* s)
{
	s->SetVector3f("material.ambient", ambient, true);
	glActiveTexture(GL_TEXTURE0);
	map->Bind();
}
void mesh_renderer_system::Init()
{
	for (const auto& entity : mEntities)
	{
		auto& renderer = gCoordinator.GetComponent<mesh_renderer>(entity);
		renderer.mat->Init(renderer.shader);
	}
}
void mesh_renderer_system::RenderUsingShader(shader* s)
{
	for (const auto& entity : mEntities)
	{
		auto& renderer = gCoordinator.GetComponent<mesh_renderer>(entity);
		auto& t = gCoordinator.GetComponent<transform>(entity);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, t.position);
		model = glm::rotate(model, t.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, t.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, t.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, t.scale);
		s->SetMatrix4("model", model, true);
		renderer.m->Render(s);
	}
}
void mesh_renderer_system::Render()
{
	for (const auto& entity : mEntities)
	{
		auto& renderer = gCoordinator.GetComponent<mesh_renderer>(entity);
		auto& t = gCoordinator.GetComponent<transform>(entity);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, t.position);
		model = glm::rotate(model, t.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, t.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, t.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, t.scale);
		renderer.shader->SetMatrix4("model", model, true);
		renderer.mat->Set(renderer.shader);
		renderer.m->Render(renderer.shader);
	}
}
