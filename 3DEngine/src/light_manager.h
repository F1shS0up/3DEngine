#pragma once
#include "ecs/System.hpp"
#include "glm/glm.hpp"
#include "transform.h"

#include <string>

#define MAX_POINT_LIGHTS 8
#define POINT_SHADOW_MAP_WIDTH 1024
#define POINT_SHADOW_MAP_HEIGHT 1024

#define DIRECTIONAL_SHADOW_MAP_WIDTH 4096
#define DIRECTIONAL_SHADOW_MAP_HEIGHT 4096

struct directional_light
{
	directional_light() = default;
	directional_light(glm::vec3 direction, float nearPlane, float farPlane, float range, float lightStrength, bool castShadows = true, glm::vec3 color = glm::vec3(0.15f, 0.15f, 0.09f));
	glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
	float nearPlane, farPlane;
	float range = 10.0f;
	float lightStrength; // multiplier
	bool castShadows = true;

	glm::vec3 color = glm::vec3(0.15f, 0.15f, 0.09f);

	glm::mat4 view;
	glm::vec3 position;
};
struct point_light
{
	float lightStrength; // multiplier
	bool castShadows = true;
	glm::vec3 color = glm::vec3(0.1f);

	int shadowMapLevel;
	float near = .1f, far = 100;
};
struct point_light_manager : public System
{
	unsigned int SHADOW_MAP_ARRAY;
	unsigned int SHADOW_MAP_FBO;

	void Init();
	void SetShaderVariables();
	void RenderFromLightsPOV();
	void SetShaderPerLightVariables(class shader* s, std::string& name, point_light& light, transform& t);
	void SetDepthShaderVariables(point_light& light, transform& t);
};

struct directional_light_manager : public System
{
	unsigned int DIRECTIONAL_DEPTH_FBO, DIRECTIONAL_DEPTH_MAP;

	void Init();
	void RenderFromLightsPOV();
	void SetLightSpaceMatrix();
	void SetShaderVariables();
	glm::mat4 GetLightSpaceMatrix();

private:
};