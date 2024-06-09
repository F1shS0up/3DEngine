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
	s->SetInteger("material.albedoMap", 0, true);
	s->SetInteger("material.normalMap", 1);
	s->SetInteger("material.metallicMap", 2);
	s->SetInteger("material.roughnessMap", 3);
	s->SetInteger("material.aoMap", 4);
	if (albedoMap == nullptr)
	{
		albedoMap = resource_manager::GetTexture("DEFAULT");
	}
	if (metallicMap == nullptr)
	{
		metallicMap = resource_manager::GetTexture("DEFAULT");
	}
	if (roughnessMap == nullptr)
	{
		roughnessMap = resource_manager::GetTexture("DEFAULT");
	}
	if (aoMap == nullptr)
	{
		aoMap = resource_manager::GetTexture("DEFAULT");
	}
	if (normalMap == nullptr)
	{
		normalMap = resource_manager::GetTexture("DEFAULT_NORMAL_MAP");
	}
}

void material_lit::Set(shader* s)
{
	s->SetFloat("uvMultiplier", uvMultiplier, true);
	s->SetVector3f("material.albedo", albedo);
	s->SetFloat("material.metallic", metallic);
	s->SetFloat("material.roughness", roughness);
	glActiveTexture(GL_TEXTURE0);
	albedoMap->Bind();
	glActiveTexture(GL_TEXTURE1);
	normalMap->Bind();
	glActiveTexture(GL_TEXTURE2);
	metallicMap->Bind();
	glActiveTexture(GL_TEXTURE3);
	roughnessMap->Bind();
	glActiveTexture(GL_TEXTURE4);
	aoMap->Bind();
}

void material_unlit::Init(shader* s)
{
	s->SetInteger("material.map", 0, true);
}

void material_unlit::Set(shader* s)
{
	s->SetFloat("uvMultiplier", uvMultiplier, true);
	s->SetVector3f("material.ambient", ambient);
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
