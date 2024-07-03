#pragma once
#include "mesh.h"
struct obj_vertex
{
	unsigned int v, t, n;
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;
	bool operator==(const obj_vertex& other) const
	{
		return v == other.v && t == other.t && n == other.n;
	}
};
struct model
{
	std::vector<mesh*> meshes;
	model() = default;
	static model LoadModelFromObj(const char* path, float verticesScalar = 1)
	{
		model m;
		m.LoadModelObj(path, verticesScalar);
		return m;
	};
	static model LoadModelFromBinary(const char* binaryFile)
	{
		model m;
		m.LoadModelBinary(binaryFile);
		return m;
	};
	static model LoadModelFromGLTF(const char* gltfFile, float verticesScalar = 1)
	{
		model m;
		m.LoadModelGLTF(gltfFile, verticesScalar);
		return m;
	};

private:
	bool LoadModelObj(const char* path, float verticesScalar);
	bool LoadModelBinary(const char* binaryFile);
	bool LoadModelGLTF(const char* binaryFile, float verticesScalar);
	void PassDataToMesh(std::vector<unsigned int>& indices, mesh* activeMesh, std::vector<obj_vertex>& objVertices);
};