
#pragma once
#include "glm/glm.hpp"

#include <vector>

struct material
{
	virtual void Init() = 0;
	virtual void Set(int index) = 0;
	virtual void AddTexturesToArrays(int index, std::vector<unsigned int> textureArrays) = 0;
	virtual void FillData(int byteOffset, char* data) = 0;
	virtual unsigned long long GetMaterialStorageSize() = 0;
	virtual std::vector<const char*> GetTextureArraysNames() = 0;
	class shader* s;
};
struct material_pbr : public material
{
	void Init() override;
	void Set(int index) override;
	void AddTexturesToArrays(int index, std::vector<unsigned int> textureArrays) override;
	void FillData(int byteOffset, char* data) override;
	unsigned long long GetMaterialStorageSize() override;
	std::vector<const char*> GetTextureArraysNames() override;

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
	void AddTexturesToArrays(int index, std::vector<unsigned int> textureArrays) override;
	void FillData(int byteOffset, char* data) override;
	unsigned long long GetMaterialStorageSize() override;
	std::vector<const char*> GetTextureArraysNames() override;

	glm::vec3 diffuse = glm::vec3(.5f);
	class texture2D* diffuseMap = nullptr;
	glm::vec3 specular = glm::vec3(1.f);
	class texture2D* specularMap = nullptr;
	float shininess = 1.0f;
	class texture2D* normalMap = nullptr;
};
struct material_lit_no_textures : public material
{
	void Init() override;
	void Set(int index) override;
	void AddTexturesToArrays(int index, std::vector<unsigned int> textureArrays) override;
	void FillData(int byteOffset, char* data) override;
	unsigned long long GetMaterialStorageSize() override;
	std::vector<const char*> GetTextureArraysNames() override;

	glm::vec3 diffuse = glm::vec3(.5f);
	glm::vec3 specular = glm::vec3(1.f);
	float shininess = 1.0f;
};
struct material_unlit : public material
{
	void Init() override;
	void Set(int index) override;
	void AddTexturesToArrays(int index, std::vector<unsigned int> textureArrays) override;
	void FillData(int byteOffset, char* data) override;
	unsigned long long GetMaterialStorageSize() override;
	std::vector<const char*> GetTextureArraysNames() override;

	glm::vec3 ambient = glm::vec3(1.0f);
	class texture2D* map = nullptr;
	float uvMultiplier = 1.0f;
};
struct transparent_pbr : public material
{
	void Init() override;
	void Set(int index) override;
	void AddTexturesToArrays(int index, std::vector<unsigned int> textureArrays) override;
	void FillData(int byteOffset, char* data) override;
	unsigned long long GetMaterialStorageSize() override;
	std::vector<const char*> GetTextureArraysNames() override;

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
