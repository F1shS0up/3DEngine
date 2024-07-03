#include "material.h"

#include "resource_manager.h"
#include "shader.h"
#include "texture.h"

void material_pbr::Init()
{
	s = resource_manager::GetShader("default_pbr");
	shaderID = s->ID;
	// s->SetInteger("material.albedoMap", 0, true);
	// s->SetInteger("material.normalMap", 1);
	// s->SetInteger("material.metallicMap", 2);
	// s->SetInteger("material.roughnessMap", 3);
	// s->SetInteger("material.aoMap", 4);
	// s->SetInteger("material.heightMap", 5);
	if (albedoMap == nullptr)
	{
		albedoMap = resource_manager::GetTexture("DEFAULT");
	}
	if (metallicMap == nullptr)
	{
		metallicMap = resource_manager::GetTexture("DEFAULT");
	}
	if (roughnessMap == nullptr)
	{
		roughnessMap = resource_manager::GetTexture("DEFAULT");
	}
	if (aoMap == nullptr)
	{
		aoMap = resource_manager::GetTexture("DEFAULT");
	}
	if (normalMap == nullptr)
	{
		normalMap = resource_manager::GetTexture("DEFAULT_NORMAL_MAP");
	}
	if (heightMap == nullptr)
	{
		heightMap = resource_manager::GetTexture("DEFAULT");
	}
}

void material_pbr::Set(int index)
{
	s->SetFloat("uvMultiplier", uvMultiplier, true);
	s->SetVector3f("material.albedo", albedo);
	s->SetFloat("material.metallic", metallic);
	s->SetFloat("material.roughness", roughness);
	s->SetFloat("material.heightScale", heightScale);
	glActiveTexture(GL_TEXTURE0);
	albedoMap->Bind();
	glActiveTexture(GL_TEXTURE1);
	normalMap->Bind();
	glActiveTexture(GL_TEXTURE2);
	metallicMap->Bind();
	glActiveTexture(GL_TEXTURE3);
	roughnessMap->Bind();
	glActiveTexture(GL_TEXTURE4);
	aoMap->Bind();
	glActiveTexture(GL_TEXTURE5);
	heightMap->Bind();
}

/* void material_pbr::AddTexturesToArrays(int index, std::vector<unsigned int> textureArrays)
{
	if (textureArrays.size() < 6)
	{
		std::cout << "Not enough texture arrays to add textures to" << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrays[0]);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, albedoMap->Width, albedoMap->Height, 1, GL_RGB, GL_UNSIGNED_BYTE, albedoMap->data);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrays[1]);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, normalMap->Width, normalMap->Height, 1, GL_RGB, GL_UNSIGNED_BYTE, normalMap->data);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrays[2]);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, metallicMap->Width, metallicMap->Height, 1, GL_RGB, GL_UNSIGNED_BYTE, metallicMap->data);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrays[3]);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, roughnessMap->Width, roughnessMap->Height, 1, GL_RGB, GL_UNSIGNED_BYTE, roughnessMap->data);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrays[4]);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, aoMap->Width, aoMap->Height, 1, GL_RGB, GL_UNSIGNED_BYTE, aoMap->data);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrays[5]);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, heightMap->Width, heightMap->Height, 1, GL_RGB, GL_UNSIGNED_BYTE, heightMap->data);
}

void material_pbr::FillData(int byteOffset, char* data) { }

unsigned long long material_pbr::GetMaterialStorageSize()
{
	return 0;
}

std::vector<const char*> material_pbr::GetTextureArraysNames()
{
	return std::vector<const char*>();
} */

void material_lit::Init()
{
	if (s == nullptr)
	{
		s = resource_manager::GetShader("default_lit");
		shaderID = s->ID;
	}
	// s->SetInteger("material.diffuseMap", 0, true);
	// s->SetInteger("material.specularMap", 1);
	// s->SetInteger("material.normalMap", 2);

	if (diffuseMap == nullptr)
	{
		diffuseMap = resource_manager::GetTexture("DEFAULT");
	}
	if (specularMap == nullptr)
	{
		specularMap = resource_manager::GetTexture("DEFAULT");
	}

	if (normalMap == nullptr)
	{
		normalMap = resource_manager::GetTexture("DEFAULT_NORMAL_MAP");
	}
}

void material_lit::Set(int index)
{
	s->Use();
	s->SetVector3f("material.diffuse", diffuse);
	s->SetVector3f("material.specular", specular);
	s->SetFloat("material.shininess", shininess);
	glActiveTexture(GL_TEXTURE0);
	diffuseMap->Bind();
	glActiveTexture(GL_TEXTURE1);
	specularMap->Bind();
	glActiveTexture(GL_TEXTURE2);
	normalMap->Bind();
}

/* void material_lit::AddTexturesToArrays(int index, std::vector<unsigned int> textureArrays)
{
	if (textureArrays.size() < 3)
	{
		std::cout << "Not enough texture arrays to add textures to" << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrays[0]);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, diffuseMap->Width, diffuseMap->Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, diffuseMap->data);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrays[1]);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, specularMap->Width, specularMap->Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, specularMap->data);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrays[2]);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, normalMap->Width, normalMap->Height, 1, GL_RGBA, GL_UNSIGNED_BYTE, normalMap->data);
}
void material_lit::FillData(int byteOffset, char* data)
{
	struct uniform_data
	{
		alignas(16) glm::vec3 diffuse;
		alignas(16) glm::vec3 specular;
		float shininess;
	};
	uniform_data dat {diffuse, specular, shininess};

	memcpy(&data[byteOffset], &dat, sizeof(uniform_data));
}
unsigned long long material_lit::GetMaterialStorageSize()
{
	struct uniform_data
	{
		alignas(16) glm::vec3 diffuse;
		alignas(16) glm::vec3 specular;
		float shininess;
	};
	return sizeof(uniform_data);
}
std::vector<const char*> material_lit::GetTextureArraysNames()
{
	return std::vector<const char*> {"materialTextures.diffuseMaps", "materialTextures.specularMaps", "materialTextures.normalMaps"};
} */
void material_lit_no_textures::Init()
{
	s = resource_manager::GetShader("default_lit_no_textures");
	shaderID = s->ID;
}
void material_lit_no_textures::Set(int index)
{
	s->Use();
	s->SetVector3f("material.diffuse", diffuse);
	s->SetVector3f("material.specular", specular);
	s->SetFloat("material.shininess", shininess);
}
/* void material_lit_no_textures::AddTexturesToArrays(int index, std::vector<unsigned int> textureArrays) { }
void material_lit_no_textures::FillData(int byteOffset, char* data)
{
	struct uniform_data
	{
		alignas(16) glm::vec3 diffuse;
		alignas(16) glm::vec3 specular;
		float shininess;
	};
	uniform_data dat {diffuse, specular, shininess};

	memcpy(&data[byteOffset], &dat, sizeof(uniform_data));
}
unsigned long long material_lit_no_textures::GetMaterialStorageSize()
{
	struct uniform_data
	{
		alignas(16) glm::vec3 diffuse;
		alignas(16) glm::vec3 specular;
		float shininess;
	};
	return sizeof(uniform_data);
}
std::vector<const char*> material_lit_no_textures::GetTextureArraysNames()
{
	return std::vector<const char*>();
} */
void material_unlit::Init()
{
	s = resource_manager::GetShader("default_unlit");
	shaderID = s->ID;
	// s->SetInteger("material.map", 0, true);
}

void material_unlit::Set(int index)
{
	s->SetFloat("uvMultiplier", uvMultiplier, true);
	s->SetVector3f("material.ambient", ambient);
	glActiveTexture(GL_TEXTURE0);
	map->Bind();
}

/* void material_unlit::AddTexturesToArrays(int index, std::vector<unsigned int> textureArrays) { }

void material_unlit::FillData(int byteOffset, char* data) { }

unsigned long long material_unlit::GetMaterialStorageSize()
{
	return 0;
}

std::vector<const char*> material_unlit::GetTextureArraysNames()
{
	return std::vector<const char*>();
} */

void transparent_pbr::Init()
{
	s = resource_manager::GetShader("transparent_pbr");
	shaderID = s->ID;
	// s->SetInteger("material.albedoMap", 0, true);
	// s->SetInteger("material.normalMap", 1);
	// s->SetInteger("material.metallicMap", 2);
	// s->SetInteger("material.roughnessMap", 3);
	// s->SetInteger("material.aoMap", 4);
	// s->SetInteger("material.heightMap", 5);
	if (albedoMap == nullptr)
	{
		albedoMap = resource_manager::GetTexture("DEFAULT");
	}
	if (metallicMap == nullptr)
	{
		metallicMap = resource_manager::GetTexture("DEFAULT");
	}
	if (roughnessMap == nullptr)
	{
		roughnessMap = resource_manager::GetTexture("DEFAULT");
	}
	if (aoMap == nullptr)
	{
		aoMap = resource_manager::GetTexture("DEFAULT");
	}
	if (normalMap == nullptr)
	{
		normalMap = resource_manager::GetTexture("DEFAULT_NORMAL_MAP");
	}
	if (heightMap == nullptr)
	{
		heightMap = resource_manager::GetTexture("DEFAULT");
	}
}

void transparent_pbr::Set(int index)
{
	s->SetFloat("uvMultiplier", uvMultiplier, true);
	s->SetVector3f("material.albedo", albedo);
	s->SetFloat("material.metallic", metallic);
	s->SetFloat("material.roughness", roughness);
	s->SetFloat("material.heightScale", heightScale);
	s->SetFloat("material.alpha", alpha);
	glActiveTexture(GL_TEXTURE0);
	albedoMap->Bind();
	glActiveTexture(GL_TEXTURE1);
	normalMap->Bind();
	glActiveTexture(GL_TEXTURE2);
	metallicMap->Bind();
	glActiveTexture(GL_TEXTURE3);
	roughnessMap->Bind();
	glActiveTexture(GL_TEXTURE4);
	aoMap->Bind();
	glActiveTexture(GL_TEXTURE5);
	heightMap->Bind();
}
/*
void transparent_pbr::AddTexturesToArrays(int index, std::vector<unsigned int> textureArrays) { }

void transparent_pbr::FillData(int byteOffset, char* data) { }

unsigned long long transparent_pbr::GetMaterialStorageSize()
{
	return 0;
}

std::vector<const char*> transparent_pbr::GetTextureArraysNames()
{
	return std::vector<const char*>();
} */
