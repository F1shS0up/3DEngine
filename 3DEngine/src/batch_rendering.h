#pragma once
#include "ecs/System.hpp"
#include "material.h"
#include "mesh.h"
#include "mesh_renderer.h"
#include "shader.h"
#include "transform.h"

#include <map>

struct batch
{
	batch();

	std::vector<std::pair<mesh*, transform*>> meshesInBatch; // With the same shader
	std::vector<int> meshMaterialModifiedIndexes;
	std::vector<material*> materials;
	shader* s;
	int vertexCount = 0;
	vertex* batchVertices;
	int indexCount = 0;
	unsigned int* batchIndices;
	unsigned int VBO, EBO, VAO;
	unsigned int materialSSBO;
	bool initialized = false;
	std::vector<unsigned int> textureArrays;
	std::vector<const char*> textureArrayNames;
	void Initialize(int textureArrayCount);
	void NewData();
	void UpdateData();
	void Render(shader* s);

private:
	void ConfigureMaterialSSBO();
	void ConfigureVerticesAndIndices();
};

struct batch_rendering_system : public System
{
	std::unordered_map<int, batch> batches;
	std::set<unsigned int> shaderIDsUsing;
	void OnEntityAdded(Entity entity) override;
	void Render();
	void RenderShadowMap(class shader* s);
};