#include "mesh_renderer.h"

#include "ecs/coordinator.hpp"
#include "engine.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/norm.hpp"
#include "material.h"
#include "resource_manager.h"
#include "shader.h"
#include "texture.h"
#include "transform.h"

#include <iostream>

extern Coordinator gCoordinator;

void mesh_renderer_system::Init()
{
	for (const auto& entity : mEntities)
	{
		auto& renderer = gCoordinator.GetComponent<mesh_renderer>(entity);
		for (auto& mesh : renderer.m->meshes)
		{
			if (mesh->materialIndex >= renderer.materials.size()) mesh->materialIndex = 0;

			renderer.materials[mesh->materialIndex]->Init();
		}
	}
}
void mesh_renderer_system::RenderShadowMap(shader* s)
{
	for (const auto& entity : mEntities)
	{
		auto& renderer = gCoordinator.GetComponent<mesh_renderer>(entity);
		auto& t = gCoordinator.GetComponent<transform>(entity);

		glm::mat4 model = t.GetModelMatrix();
		s->SetMatrix4("model", model, true);
		for (auto& mesh : renderer.m->meshes)
		{
			if (mesh->castsShadow == false) continue;
			mesh->Render(s);
		}
	}
}
bool CompareMeshPositions(const mesh* a, const mesh* b)
{
	return a->sqrDistance > b->sqrDistance;
}

bool CompareMeshRenderOrder(const Entity a, const Entity b)
{
	auto& renderera = gCoordinator.GetComponent<mesh_renderer>(a);
	auto& rendererb = gCoordinator.GetComponent<mesh_renderer>(b);

	return renderera.sqrDistance > rendererb.sqrDistance;
}
void mesh_renderer_system::Update()
{
	float time = glfwGetTime();
	orderedRenders.clear();
	std::copy(mEntities.begin(), mEntities.end(), std::back_inserter(orderedRenders));
	for (const auto& entity : mEntities)
	{
		auto& renderer = gCoordinator.GetComponent<mesh_renderer>(entity);
		auto& t = gCoordinator.GetComponent<transform>(entity);

		renderer.sqrDistance = glm::length2(engine::Instance()->cameraSystem.GetCurrentCamera()->position - t.GetGlobalPosition());

		for (auto& m : renderer.m->meshes)
		{
			if (!m->bv->IsOnFrustum(engine::Instance()->cameraSystem.GetCurrentCamera()->cameraFrustum, t))
			{
				renderer.visibleMeshes[m->name] = true;
			}
			else
			{
				renderer.visibleMeshes[m->name] = true;
			}
			m->sqrDistance = glm::length2(engine::Instance()->cameraSystem.GetCurrentCamera()->position - m->averagePosition * t.GetModelMatrix());
		}

		std::sort(renderer.m->meshes.begin(), renderer.m->meshes.end(), CompareMeshPositions);
	}

	std::sort(orderedRenders.begin(), orderedRenders.end(), CompareMeshRenderOrder);
	float endTime = glfwGetTime();
}
void mesh_renderer_system::Render(bool transparentPass)
{
	float time = glfwGetTime();
	for (const auto& entity : orderedRenders)
	{
		auto& renderer = gCoordinator.GetComponent<mesh_renderer>(entity);

		auto& t = gCoordinator.GetComponent<transform>(entity);

		glm::mat4 model = t.GetModelMatrix();
		for (auto& mesh : renderer.m->meshes)
		{
			if (renderer.visibleMeshes[mesh->name] == false) continue;
			if ((dynamic_cast<transparent_pbr*>(renderer.materials[mesh->materialIndex]) == nullptr) == transparentPass)
			{
				continue;
			}
			renderer.materials[mesh->materialIndex]->s->SetMatrix4("model", model, true);
			renderer.materials[mesh->materialIndex]->Set(0);
			mesh->Render(renderer.materials[mesh->materialIndex]->s);
		}
	}
	float endTime = glfwGetTime();
}
