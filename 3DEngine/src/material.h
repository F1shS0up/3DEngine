
#pragma once
#include "glm/glm.hpp"
struct material
{
	virtual void Init() = 0;
	virtual void Set() = 0;
	class shader* s;
};
struct material_lit : public material
{
	void Init() override;
	void Set() override;
	class texture2D* albedoMap;
	glm::vec3 albedo = glm::vec3(1.0f);
	class texture2D* normalMap;
	class texture2D* metallicMap;
	float metallic = 1.0f;
	class texture2D* roughnessMap;
	float roughness = 1.0f;
	class texture2D* aoMap;
	float uvMultiplier = 1.0f;
	class texture2D* heightMap;
	float heightScale = 0.0f;
};
struct transparent_lit : public material
{
	void Init() override;
	void Set() override;
	class texture2D* albedoMap;
	glm::vec3 albedo = glm::vec3(1.0f);
	class texture2D* normalMap;
	class texture2D* metallicMap;
	float metallic = 1.0f;
	class texture2D* roughnessMap;
	float roughness = 1.0f;
	class texture2D* aoMap;
	float uvMultiplier = 1.0f;
	class texture2D* heightMap;
	float heightScale = 0.0f;
	float alpha = .5f;
};
struct material_unlit : public material
{
	void Init() override;
	void Set() override;
	glm::vec3 ambient = glm::vec3(1.0f);
	class texture2D* map = nullptr;
	float uvMultiplier = 1.0f;
};
