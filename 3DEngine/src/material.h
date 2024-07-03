
#pragma once
#include "glm/glm.hpp"

#include <vector>

struct material
{
	virtual ~material() = default;
	virtual void Init() = 0;
	virtual void Set(int index) = 0;
	virtual const char* GetName() = 0;
	// virtual void AddTexturesToArrays(int index, std::vector<unsigned int> textureArrays) = 0;
	// virtual void FillData(int byteOffset, char* data) = 0;
	// virtual unsigned long long GetMaterialStorageSize() = 0;
	// virtual std::vector<const char*> GetTextureArraysNames() = 0;
	class shader* s;
	unsigned int shaderID = -1;
};
struct material_pbr : public material
{
	material_pbr()
	{
		Init();
	}
	~material_pbr() override = default;
	void Set(int index) override;
	void Init() override;
	const char* GetName() override
	{
		return "material_pbr";
	}
	// void AddTexturesToArrays(int index, std::vector<unsigned int> textureArrays) override;
	// void FillData(int byteOffset, char* data) override;
	// unsigned long long GetMaterialStorageSize() override;
	// std::vector<const char*> GetTextureArraysNames() override;

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
	material_lit()
	{
		Init();
	}
	~material_lit() override = default;
	void Set(int index) override;
	void Init() override;
	const char* GetName() override
	{
		return "material_lit";
	}
	// void AddTexturesToArrays(int index, std::vector<unsigned int> textureArrays) override;
	// void FillData(int byteOffset, char* data) override;
	// unsigned long long GetMaterialStorageSize() override;
	// std::vector<const char*> GetTextureArraysNames() override;

	glm::vec3 diffuse = glm::vec3(.5f);
	class texture2D* diffuseMap = nullptr;
	glm::vec3 specular = glm::vec3(1.f);
	class texture2D* specularMap = nullptr;
	float shininess = 1.0f;
	class texture2D* normalMap = nullptr;
};
struct material_lit_no_textures : public material
{
	material_lit_no_textures()
	{
		Init();
	}
	~material_lit_no_textures() override = default;
	void Set(int index) override;
	void Init() override;
	const char* GetName() override
	{
		return "material_lit_no_textures";
	}
	// void AddTexturesToArrays(int index, std::vector<unsigned int> textureArrays) override;
	// void FillData(int byteOffset, char* data) override;
	// unsigned long long GetMaterialStorageSize() override;
	// std::vector<const char*> GetTextureArraysNames() override;

	glm::vec3 diffuse = glm::vec3(.5f);
	glm::vec3 specular = glm::vec3(1.f);
	float shininess = 1.0f;
};
struct material_unlit : public material
{
	material_unlit()
	{
		Init();
	}
	~material_unlit() override = default;
	void Set(int index) override;
	void Init() override;
	const char* GetName() override
	{
		return "material_unlit";
	}
	// void AddTexturesToArrays(int index, std::vector<unsigned int> textureArrays) override;
	// void FillData(int byteOffset, char* data) override;
	// unsigned long long GetMaterialStorageSize() override;
	// std::vector<const char*> GetTextureArraysNames() override;

	glm::vec3 ambient = glm::vec3(1.0f);
	class texture2D* map = nullptr;
	float uvMultiplier = 1.0f;
};
struct transparent_pbr : public material
{
	transparent_pbr()
	{
		Init();
	}
	~transparent_pbr() override = default;
	void Set(int index) override;
	void Init() override;
	const char* GetName() override
	{
		return "transparent_pbr";
	}
	// void AddTexturesToArrays(int index, std::vector<unsigned int> textureArrays) override;
	// void FillData(int byteOffset, char* data) override;
	// unsigned long long GetMaterialStorageSize() override;
	// std::vector<const char*> GetTextureArraysNames() override;

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
