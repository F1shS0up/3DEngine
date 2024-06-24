
#pragma once
#include "glm/glm.hpp"

#include <vector>

struct material
{
	virtual void Init() = 0;
	virtual void Set(int index) = 0;
	class shader* s;
};
struct material_pbr : public material
{
	void Init() override;
	void Set(int index) override;
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
struct material_lit : public material
{
	void Init() override;
	void Set(int index) override;
	glm::vec3 ambient = glm::vec3(.1f);
	glm::vec3 diffuse = glm::vec3(.5f);
	class texture2D* diffuseMap = nullptr;
	glm::vec3 specular = glm::vec3(1.f);
	class texture2D* specularMap = nullptr;
	float shininess = 1.0f;
	class texture2D* normalMap = nullptr;
};
struct transparent_pbr : public material
{
	void Init() override;
	void Set(int index) override;
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
	void Set(int index) override;
	glm::vec3 ambient = glm::vec3(1.0f);
	class texture2D* map = nullptr;
	float uvMultiplier = 1.0f;
};
