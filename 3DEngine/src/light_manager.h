#pragma once
#include "ecs/System.hpp"
#include "glm/glm.hpp"
#include "transform.h"

#include <iostream>
#include <string>

#define POINT_SHADOW_MAP_WIDTH 1024
#define POINT_SHADOW_MAP_HEIGHT 1024

#define DIRECTIONAL_SHADOW_MAP_WIDTH 4096
#define DIRECTIONAL_SHADOW_MAP_HEIGHT 4096

struct uniform_light
{
	alignas(16) glm::vec3 positionOrDirection;
	alignas(16) glm::vec3 ambient;
	alignas(16) glm::vec3 diffuse;
	alignas(16) glm::vec3 specular;

	float farPlane;

	int castShadows;
};
struct light
{
	enum light_type
	{
		POINT,
		DIRECTIONAL
	};

	light() = default;
	light(float nearPlane, float farPlane, float lightStrength, light_type type, bool castShadows = true, glm::vec3 ambient = glm::vec3(.1f, .1f, .1f), glm::vec3 diffuse = glm::vec3(.5f, .5f, .5f),
		  glm::vec3 specular = glm::vec3(1.f, 1.f, 1.f)) :
		nearPlane(nearPlane), farPlane(farPlane), lightStrength(lightStrength), castShadows(castShadows), ambient(ambient), diffuse(diffuse), specular(specular), type(type) {};
	~light() {};
	float nearPlane, farPlane;
	float lightStrength; // multiplier
	bool castShadows = true;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	light_type type = DIRECTIONAL;
};

struct directional_light
{
	directional_light() = default;
	directional_light(glm::vec3 direction, float range);

	~directional_light() {};

	glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
	float range = 10.0f;

	glm::mat4 view;
	glm::vec3 position;
};
struct point_light
{
	point_light() = default;
	~point_light() {};

	int shadowMapLevel;
};
struct point_light_manager : public System
{
	unsigned int SHADOW_MAP_ARRAY;
	unsigned int SHADOW_MAP_FBO;

	void Init();
	void RenderFromLightsPOV();
	void SetDepthShaderVariables(point_light& pl, light& l, transform& t);

	void LinkShadowMap();
};

struct directional_light_manager : public System
{
	unsigned int DIRECTIONAL_DEPTH_FBO, DIRECTIONAL_DEPTH_MAP;

	void Init();
	void RenderFromLightsPOV();
	void SetLightSpaceMatrix();

	glm::mat4 GetLightSpaceMatrix();

	void LinkShadowMap();
};

struct light_manager : public System
{
	unsigned int SSBO;
	void Init();
	void SetShaderVariables();
};