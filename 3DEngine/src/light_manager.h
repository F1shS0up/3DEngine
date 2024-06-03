#pragma once
#include "ecs/System.hpp"
#include "glm/glm.hpp"
#include "transform.h"

#include <string>

struct directional_light
{
	directional_light() = default;
	directional_light(glm::vec3 direction, float nearPlane, float farPlane, float range, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
	float nearPlane, farPlane;
	float range = 10.0f;

	glm::vec3 ambient = glm::vec3(0.1f);
	glm::vec3 diffuse = glm::vec3(0.5f);
	glm::vec3 specular = glm::vec3(1.0f);

	glm::mat4 view;

	glm::vec3 position;
};
struct point_light
{
	float constant;
	float linear;
	float quadratic;

	glm::vec3 ambient = glm::vec3(0.1f);
	glm::vec3 diffuse = glm::vec3(0.5f);
	glm::vec3 specular = glm::vec3(1.0f);

	float near = 1.f, far = 100.f;

	int SHADOW_MAP_WIDTH = 1024, SHADOW_MAP_HEIGHT = 1024;
	unsigned int DEPTH_FBO, DEPTH_CUBEMAP;
};
struct point_light_manager : public System
{
	void Init();
	void Update();
	void RenderFromLightsPOV();
	void SetShaderVariables(class shader* s, std::string& name, point_light& light, transform& t);
	void SetDepthShaderVariables(point_light& light, transform& t);
};

struct directional_light_manager : public System
{
	glm::mat4 GetLightSpaceMatrix();
};