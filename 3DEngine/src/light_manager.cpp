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
	glGenTextures(1, &SHADOW_MAP_ARRAY);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, SHADOW_MAP_ARRAY);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT, POINT_SHADOW_MAP_WIDTH, POINT_SHADOW_MAP_HEIGHT, 6 * MAX_POINT_LIGHTS, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);

	glGenFramebuffers(1, &SHADOW_MAP_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, SHADOW_MAP_FBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, SHADOW_MAP_ARRAY, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	int index = 0;
	for (const auto& entity : mEntities)
	{
		auto& light = gCoordinator.GetComponent<point_light>(entity);
		light.shadowMapLevel = index;
		index++;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void point_light_manager::SetShaderVariables()
{
	resource_manager::GetShader("default_pbr")->SetInteger("lightCount", mEntities.size() + 1, true); // +1 for directional light
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, SHADOW_MAP_ARRAY);
	resource_manager::GetShader("default_lit")->SetInteger("lightCount", mEntities.size() + 1, true); // +1 for directional light
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, SHADOW_MAP_ARRAY);
	resource_manager::GetShader("transparent_pbr")->SetInteger("lightCount", mEntities.size() + 1, true); // +1 for directional light
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, SHADOW_MAP_ARRAY);

	int index = 0;
	for (const auto& entity : mEntities)
	{
		point_light& light = gCoordinator.GetComponent<point_light>(entity);
		transform& t = gCoordinator.GetComponent<transform>(entity);
		std::string name = "lights[" + std::to_string(index + 1) + "].";
		shader* s = resource_manager::GetShader("default_pbr");
		SetShaderPerLightVariables(s, true, name, light, t);

		s = resource_manager::GetShader("default_lit");
		SetShaderPerLightVariables(s, false, name, light, t);

		s = resource_manager::GetShader("transparent_pbr");
		SetShaderPerLightVariables(s, true, name, light, t);
		index++;
	}
}

void point_light_manager::RenderFromLightsPOV()
{
	glViewport(0, 0, POINT_SHADOW_MAP_WIDTH, POINT_SHADOW_MAP_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, SHADOW_MAP_FBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	for (const auto& entity : mEntities)
	{
		point_light& light = gCoordinator.GetComponent<point_light>(entity);
		transform& t = gCoordinator.GetComponent<transform>(entity);
		SetDepthShaderVariables(light, t);
		meshRendererSystem->RenderShadowMap(resource_manager::GetShader("POINT_SHADOW_MAPPING"));
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void point_light_manager::SetShaderPerLightVariables(shader* s, bool useOnlyOneColor, std::string& name, point_light& light, transform& t)
{
	if (useOnlyOneColor)
	{
		s->SetVector3f((name + "color").c_str(), light.specular * light.lightStrength);
	}
	else
	{
		s->SetVector3f((name + "ambient").c_str(), light.ambient * light.lightStrength);
		s->SetVector3f((name + "diffuse").c_str(), light.diffuse * light.lightStrength);
		s->SetVector3f((name + "specular").c_str(), light.specular * light.lightStrength);
	}

	s->SetVector3f((name + "positionOrDirection").c_str(), t.GetGlobalPosition(), true);
	s->SetFloat((name + "farPlane").c_str(), light.far);
	s->SetInteger((name + "castShadows").c_str(), light.castShadows);
}

void point_light_manager::SetDepthShaderVariables(point_light& light, transform& t)
{
	resource_manager::GetShader("POINT_SHADOW_MAPPING")->Use();
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)POINT_SHADOW_MAP_WIDTH / (float)POINT_SHADOW_MAP_HEIGHT, light.near, light.far);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt((glm::vec3)t.GetGlobalPosition(), (glm::vec3)t.GetGlobalPosition() + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt((glm::vec3)t.GetGlobalPosition(), (glm::vec3)t.GetGlobalPosition() + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt((glm::vec3)t.GetGlobalPosition(), (glm::vec3)t.GetGlobalPosition() + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt((glm::vec3)t.GetGlobalPosition(), (glm::vec3)t.GetGlobalPosition() + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt((glm::vec3)t.GetGlobalPosition(), (glm::vec3)t.GetGlobalPosition() + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt((glm::vec3)t.GetGlobalPosition(), (glm::vec3)t.GetGlobalPosition() + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
	std::string prefix = "shadowMatrices[";
	for (unsigned int i = 0; i < 6; ++i)
		resource_manager::GetShader("POINT_SHADOW_MAPPING")->SetMatrix4((prefix + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
	resource_manager::GetShader("POINT_SHADOW_MAPPING")->SetFloat("farPlane", light.far);
	resource_manager::GetShader("POINT_SHADOW_MAPPING")->SetVector3f("lightPos", t.GetGlobalPosition());
	resource_manager::GetShader("POINT_SHADOW_MAPPING")->SetInteger("cubemapIndex", light.shadowMapLevel);
}

directional_light::directional_light(glm::vec3 direction, float nearPlane, float farPlane, float range, float lightStrength, bool castShadows, glm::vec3 color) :
	nearPlane(nearPlane), farPlane(farPlane), ambient(color * .1f), diffuse(color * .5f), specular(color), range(range), lightStrength(lightStrength), castShadows(castShadows)
{
	this->direction = glm::normalize(direction);
	position = -this->direction * farPlane / 2.f;
	view = glm::lookAt(position, position + this->direction, glm::vec3(0.0, 1.0, 0.0));
}

directional_light::directional_light(glm::vec3 direction, float nearPlane, float farPlane, float range, float lightStrength, bool castShadows, glm::vec3 ambient, glm::vec3 diffuse,
									 glm::vec3 specular) :
	nearPlane(nearPlane), farPlane(farPlane), ambient(ambient), diffuse(diffuse), specular(specular), range(range), lightStrength(lightStrength), castShadows(castShadows)
{
	this->direction = glm::normalize(direction);
	position = -this->direction * farPlane / 2.f;
	view = glm::lookAt(position, position + this->direction, glm::vec3(0.0, 1.0, 0.0));
}

void directional_light_manager::Init()
{
	glGenFramebuffers(1, &DIRECTIONAL_DEPTH_FBO);
	glGenTextures(1, &DIRECTIONAL_DEPTH_MAP);
	glBindTexture(GL_TEXTURE_2D, DIRECTIONAL_DEPTH_MAP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DIRECTIONAL_SHADOW_MAP_WIDTH, DIRECTIONAL_SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, DIRECTIONAL_DEPTH_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DIRECTIONAL_DEPTH_MAP, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void directional_light_manager::RenderFromLightsPOV()
{
	glViewport(0, 0, DIRECTIONAL_SHADOW_MAP_WIDTH, DIRECTIONAL_SHADOW_MAP_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, DIRECTIONAL_DEPTH_FBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	meshRendererSystem->RenderShadowMap(resource_manager::GetShader("DIRECTIONAL_SHADOW_MAPPING"));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void directional_light_manager::SetLightSpaceMatrix()
{
	glm::mat4 lightSpaceMatrix = GetLightSpaceMatrix();
	resource_manager::GetShader("DIRECTIONAL_SHADOW_MAPPING")->SetMatrix4("directionalLightSpaceMatrix", lightSpaceMatrix, true);
	resource_manager::GetShader("default_pbr")->SetMatrix4("directionalLightSpaceMatrix", lightSpaceMatrix, true);
	resource_manager::GetShader("default_lit")->SetMatrix4("directionalLightSpaceMatrix", lightSpaceMatrix, true);
	resource_manager::GetShader("default_unlit")->SetMatrix4("directionalLightSpaceMatrix", lightSpaceMatrix, true);
	resource_manager::GetShader("transparent_pbr")->SetMatrix4("directionalLightSpaceMatrix", lightSpaceMatrix, true);
}

glm::mat4 directional_light_manager::GetLightSpaceMatrix()
{
	for (const auto& entity : mEntities)
	{
		auto& light = gCoordinator.GetComponent<directional_light>(entity);
		glm::vec3 camPos = engine::Instance()->cameraSystem.GetCurrentCamera()->position;
		glm::mat4 projection = glm::ortho(-light.range, +light.range, -light.range, light.range, light.nearPlane, light.farPlane);

		return projection * light.view;
	}
	return glm::mat4(1);
}

void directional_light_manager::SetShaderVariables()
{
	for (const auto& entity : mEntities)
	{
		auto& light = gCoordinator.GetComponent<directional_light>(entity);
		std::string name = "lights[0].";
		shader* s = resource_manager::GetShader("default_lit");
		SetShaderPerLightVariables(s, false, name, light);

		s = resource_manager::GetShader("default_pbr");
		SetShaderPerLightVariables(s, true, name, light);

		s = resource_manager::GetShader("transparent_pbr");
		SetShaderPerLightVariables(s, true, name, light);

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, DIRECTIONAL_DEPTH_MAP);
		return;
	}
}

void directional_light_manager::SetShaderPerLightVariables(shader* s, bool useOnlyOneColor, std::string& name, directional_light& light)
{

	if (useOnlyOneColor)
	{
		s->SetVector3f("lights[0].color", light.specular * light.lightStrength, true);
	}
	else
	{
		s->SetVector3f("lights[0].ambient", light.ambient * light.lightStrength, true);
		s->SetVector3f("lights[0].diffuse", light.diffuse * light.lightStrength, true);
		s->SetVector3f("lights[0].specular", light.specular * light.lightStrength, true);
	}
	s->SetVector3f("lights[0].positionOrDirection", light.direction);
	s->SetInteger("lights[0].castShadows", light.castShadows);
	s->SetFloat("lights[0].farPlane", light.farPlane);
}