#include "batch_rendering.h"

#include "ecs/coordinator.hpp"
#include "glm/glm.hpp"
#include "mesh.h"

#include <set>

extern Coordinator gCoordinator;
void batch_rendering_system::OnEntityAdded(Entity entity)
{
	auto& renderer = gCoordinator.GetComponent<mesh_renderer>(entity);
	auto& t = gCoordinator.GetComponent<transform>(entity);
	std::set<unsigned int> shaderIDsUsing;
	std::set<unsigned int> shaderIDsUsingInEntity;
	for (auto& m : renderer.m->meshes)
	{
		shaderIDsUsing.insert(m->s->ID);
		shaderIDsUsingInEntity.insert(m->s->ID);
		batches[m->s->ID].s = m->s;
		batches[m->s->ID].meshesInBatch.push_back(std::pair<mesh*, transform*>(m, &t));
		batches[m->s->ID].vertexCount += m->vertexCount;
		batches[m->s->ID].indexCount += m->indexCount;
		int materialIndexOffset = batches[m->s->ID].materials.size();
		batches[m->s->ID].meshMaterialModifiedIndexes.push_back(m->materialIndex + materialIndexOffset);
	}
	for (int shader : shaderIDsUsingInEntity)
	{
		// Update batch
		for (int i = 0; i < renderer.materials.size(); i++)
		{
			batches[shader].materials.push_back(renderer.materials[i]);
		}
		batches[shader].NewData();
	}
}

void batch_rendering_system::Render()
{
	for (auto& batch : batches)
	{
		// batch.second.materials[0]->Set(0);
		batch.second.Render(batch.second.s);
	}
}

void batch_rendering_system::RenderShadowMap(shader* s)
{
	for (auto& batch : batches)
	{
		batch.second.Render(s);
	}
}

batch::batch()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float) + sizeof(int), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float) + sizeof(int), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float) + sizeof(int), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float) + sizeof(int), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float) + sizeof(int), (void*)(11 * sizeof(float)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 1, GL_INT, GL_FALSE, 14 * sizeof(float) + sizeof(int), (void*)(14 * sizeof(float)));
	glEnableVertexAttribArray(5);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	batchVertices = (vertex*)malloc(sizeof(vertex) * vertexCount);
	batchIndices = (unsigned int*)malloc(sizeof(unsigned int) * indexCount);

	glGenBuffers(1, &materialSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void batch::Initialize(int textureArrayCount)
{
	textureArrays.reserve(textureArrayCount);
	textureArrayNames.reserve(textureArrayCount);
	for (int i = 0; i < textureArrayCount; i++)
	{
		unsigned int textureArray;
		glGenTextures(1, &textureArray);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		textureArrays.push_back(textureArray);
	}
}

void batch::NewData()
{
	// We need to create a larger or smaller buffer
	ConfigureVerticesAndIndices();
	ConfigureMaterialSSBO();

	textureArrayNames = materials[0]->GetTextureArraysNames();
	if (!initialized)
	{
		Initialize(textureArrayNames.size());
		initialized = true;
	}

	for (unsigned int tex : textureArrays)
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA, 1024, 1024, 1);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}
	for (int i = 0; i < materials.size(); i++)
	{
		materials[i]->AddTexturesToArrays(i, textureArrays);
	}

	s->Use();
	for (int i = 0; i < textureArrayNames.size(); i++)
	{
		s->SetInteger(textureArrayNames[i], i);
	}
}

void batch::ConfigureMaterialSSBO()
{
	unsigned long long materialsStorageSize = materials[0]->GetMaterialStorageSize() * materials.size();
	char* data = new char[materialsStorageSize];
	for (int i = 0; i < materials.size(); i++)
	{
		materials[i]->FillData(i * materials[0]->GetMaterialStorageSize(), data);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, materialsStorageSize, data, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, materialSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, materialSSBO, 0, materialsStorageSize);
}

void batch::ConfigureVerticesAndIndices()
{
	batchVertices = (vertex*)realloc(batchVertices, sizeof(vertex) * vertexCount);
	batchIndices = (unsigned int*)realloc(batchIndices, sizeof(unsigned int) * indexCount);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	int vertexIndex = 0;
	int indexIndex = 0;
	int indicesOffset = 0;
	for (int i = 0; i < meshesInBatch.size(); i++)
	{
		for (int j = 0; j < meshesInBatch[i].first->vertexCount; j++)
		{
			batchVertices[vertexIndex] = meshesInBatch[i].first->vertices[j];
			batchVertices[vertexIndex].position = meshesInBatch[i].second->GetModelMatrix() * glm::vec4(batchVertices[vertexIndex].position, 1.0);
			batchVertices[vertexIndex].normal = meshesInBatch[i].second->GetModelMatrix() * glm::vec4(batchVertices[vertexIndex].normal, 0.0);
			batchVertices[vertexIndex].tangent = meshesInBatch[i].second->GetModelMatrix() * glm::vec4(batchVertices[vertexIndex].tangent, 0.0);
			batchVertices[vertexIndex].bitangent = meshesInBatch[i].second->GetModelMatrix() * glm::vec4(batchVertices[vertexIndex].bitangent, 0.0);
			vertexIndex++;
		}
		for (int j = 0; j < meshesInBatch[i].first->indexCount; j++)
		{
			batchIndices[indexIndex] = meshesInBatch[i].first->indices[j] + indicesOffset;
			indexIndex++;
		}
		indicesOffset += meshesInBatch[i].first->vertexCount;
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertexCount, &batchVertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexCount, &batchIndices[0], GL_STATIC_DRAW);
}

void batch::UpdateData()
{
	// We need to replace different data to the buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	int index = 0;
	for (int i = 0; i < meshesInBatch.size(); i++)
	{
		for (int j = 0; j < meshesInBatch[i].first->vertexCount; j++)
		{
			batchVertices[index] = meshesInBatch[i].first->vertices[j];
			batchVertices[index].position = glm::vec4(batchVertices[index].position, 1.0) * meshesInBatch[i].second->GetModelMatrix();
			index++;
		}
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex) * vertexCount, &batchVertices[0]);
}

void batch::Render(shader* s)
{
	s->Use();
	for (int i = 0; i < textureArrayNames.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrays[i]);
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	// We render
}
