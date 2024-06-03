#include "light_manager.h"

#include "ecs/coordinator.hpp"
#include "engine.h"
#include "resource_manager.h"
#include "shader.h"
#include "transform.h"

#include <iostream>

extern Coordinator gCoordinator;
extern std::shared_ptr<mesh_renderer_system> meshRendererSystem;
void point_light_manager::Init()
{
	for (const auto& entity : mEntities)
	{
		point_light& light = gCoordinator.GetComponent<point_light>(entity);
		glGenFramebuffers(1, &light.DEPTH_FBO);
		glGenTextures(1, &light.DEPTH_CUBEMAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, light.DEPTH_CUBEMAP);
		for (unsigned int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, light.SHADOW_MAP_WIDTH, light.SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindFramebuffer(GL_FRAMEBUFFER, light.DEPTH_FBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, light.DEPTH_CUBEMAP, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
void point_light_manager::Update()
{

	resource_manager::GetShader("default_lit")->SetInteger("pointLightCount", mEntities.size(), true);

	int index = 0;
	for (const auto& entity : mEntities)
	{
		point_light& light = gCoordinator.GetComponent<point_light>(entity);
		transform& t = gCoordinator.GetComponent<transform>(entity);
		shader* s = resource_manager::GetShader("default_lit");
		std::string name = "pointLights[" + std::to_string(index) + "].";
		SetShaderVariables(s, name, light, t);
		index++;
	}
}

void point_light_manager::RenderFromLightsPOV()
{
	for (const auto& entity : mEntities)
	{
		point_light& light = gCoordinator.GetComponent<point_light>(entity);
		transform& t = gCoordinator.GetComponent<transform>(entity);
		SetDepthShaderVariables(light, t);
		glViewport(0, 0, light.SHADOW_MAP_WIDTH, light.SHADOW_MAP_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, light.DEPTH_FBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		meshRendererSystem->RenderUsingShader(resource_manager::GetShader("POINT_SHADOW_MAPPING"));
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void point_light_manager::SetShaderVariables(shader* s, std::string& name, point_light& light, transform& t)
{
	s->SetVector3f((name + "position").c_str(), t.position, true);
	s->SetVector3f((name + "ambient").c_str(), light.ambient, true);
	s->SetVector3f((name + "diffuse").c_str(), light.diffuse, true);
	s->SetVector3f((name + "specular").c_str(), light.specular, true);

	s->SetFloat((name + "constant").c_str(), 1.0f);
	s->SetFloat((name + "linear").c_str(), 0.09f);
	s->SetFloat((name + "quadratic").c_str(), 0.032f);
}

void point_light_manager::SetDepthShaderVariables(point_light& light, transform& t)
{
	resource_manager::GetShader("POINT_SHADOW_MAPPING")->Use();
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)light.SHADOW_MAP_WIDTH / (float)light.SHADOW_MAP_HEIGHT, light.near, light.far);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(t.position, t.position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(t.position, t.position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(t.position, t.position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(t.position, t.position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(t.position, t.position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(t.position, t.position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
	std::string prefix = "shadowMatrices[";
	for (unsigned int i = 0; i < 6; ++i)
		resource_manager::GetShader("POINT_SHADOW_MAPPING")->SetMatrix4((prefix + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
	resource_manager::GetShader("POINT_SHADOW_MAPPING")->SetFloat("farPlane", light.far);
	resource_manager::GetShader("POINT_SHADOW_MAPPING")->SetVector3f("lightPos", t.position);
}

directional_light::directional_light(glm::vec3 direction, float nearPlane, float farPlane, float range, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
	nearPlane(nearPlane), farPlane(farPlane), ambient(ambient), diffuse(diffuse), specular(specular), range(range)
{
	this->direction = glm::normalize(direction);
	position = -this->direction * farPlane / 2.f;
	view = glm::lookAt(position, position + this->direction, glm::vec3(0.0, 1.0, 0.0));
}

glm::mat4 directional_light_manager::GetLightSpaceMatrix()
{
	for (const auto& entity : mEntities)
	{
		auto& light = gCoordinator.GetComponent<directional_light>(entity);
		glm::vec3 camPos = engine::Instance()->cameraSystem.GetCurrentCamera()->GetPosition();
		glm::mat4 projection = glm::ortho(camPos.x - light.range, camPos.x + light.range, camPos.z - light.range, camPos.z + light.range, light.nearPlane, light.farPlane);

		return projection * light.view;
	}
	return glm::mat4(1);
}
