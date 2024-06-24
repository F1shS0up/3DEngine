#include "material.h"

#include "resource_manager.h"
#include "shader.h"
#include "texture.h"

void material_pbr::Init()
{
	if (s == nullptr)
	{
		s = resource_manager::GetShader("default_pbr");
	}
	s->SetInteger("material.albedoMap", 0, true);
	s->SetInteger("material.normalMap", 1);
	s->SetInteger("material.metallicMap", 2);
	s->SetInteger("material.roughnessMap", 3);
	s->SetInteger("material.aoMap", 4);
	s->SetInteger("material.heightMap", 5);
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

void material_unlit::Init()
{
	if (s == nullptr)
	{
		s = resource_manager::GetShader("default_unlit");
	}
	s->SetInteger("material.map", 0, true);
}

void material_unlit::Set(int index)
{
	s->SetFloat("uvMultiplier", uvMultiplier, true);
	s->SetVector3f("material.ambient", ambient);
	glActiveTexture(GL_TEXTURE0);
	map->Bind();
}

void transparent_pbr::Init()
{
	if (s == nullptr)
	{
		s = resource_manager::GetShader("transparent_pbr");
	}
	s->SetInteger("material.albedoMap", 0, true);
	s->SetInteger("material.normalMap", 1);
	s->SetInteger("material.metallicMap", 2);
	s->SetInteger("material.roughnessMap", 3);
	s->SetInteger("material.aoMap", 4);
	s->SetInteger("material.heightMap", 5);
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

void material_lit::Init()
{
	if (s == nullptr)
	{
		s = resource_manager::GetShader("default_lit");
	}
	s->SetInteger("material.diffuseMap", 0, true);
	s->SetInteger("material.specularMap", 1);
	s->SetInteger("material.normalMap", 2);

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
	s->SetVector3f("material.ambient", ambient);
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
