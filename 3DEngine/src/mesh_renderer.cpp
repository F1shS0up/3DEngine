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

void mesh_renderer_system::OnEntityAdded(Entity entity)
{
	// auto& renderer = gCoordinator.GetComponent<mesh_renderer>(entity);
	// for (auto& mesh : renderer.m->meshes)
	//{
	//	if (mesh->materialIndex >= renderer.materials.size()) mesh->materialIndex = 0;
	//
	//	// mesh->s = renderer.materials[mesh->materialIndex]->s;
	//
	//	// if (std::find(shaderIDsUsing.begin(), shaderIDsUsing.end(), mesh->shaderID) == shaderIDsUsing.end())
	//	//{
	//	//	shaderIDsUsing.push_back(mesh->shaderID);
	//	// }
	//	//
	//	// meshesUsingShaderID[mesh->shaderID].push_back(mesh);
	//}
}
void mesh_renderer_system::RenderShadowMap(shader* s)
{
	for (const auto& entity : mEntities)
	{
		auto& renderer = gCoordinator.GetComponent<mesh_renderer>(entity);
		auto& t = gCoordinator.GetComponent<transform>(entity);

		for (auto& mesh : renderer.m->meshes)
		{
			s->Use();
			s->SetMatrix4("model", t.GetModelMatrix(), true);
			glBindVertexArray(mesh->VAO);
			glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
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

		for (int i = 0; i < renderer.m->meshes.size(); i++)
		{
			auto& m = renderer.m->meshes[i];
			if (!m->bv->IsOnFrustum(engine::Instance()->cameraSystem.GetCurrentCamera()->cameraFrustum, t))
			{
				renderer.visibleMeshes[i] = true;
			}
			else
			{
				renderer.visibleMeshes[i] = true;
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
		for (int i = 0; i < renderer.m->meshes.size(); i++)
		{
			auto& m = renderer.m->meshes[i];
			if (renderer.visibleMeshes[i] == false) continue;
			if ((dynamic_cast<transparent_pbr*>(renderer.materials[m->materialIndex]) == nullptr) == transparentPass)
			{
				continue;
			}
			if (renderer.materials[m->materialIndex]->s == nullptr) std::cout << "shader is NULL" << std::endl;
			renderer.materials[m->materialIndex]->s->SetMatrix4("model", model, true);
			renderer.materials[m->materialIndex]->Set(0);
			renderer.materials[m->materialIndex]->s->Use();
			glBindVertexArray(m->VAO);
			glDrawElements(GL_TRIANGLES, m->indexCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
	float endTime = glfwGetTime();
}
