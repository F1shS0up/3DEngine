#pragma once
#include "mesh.h"

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
	void CreateNewMesh(int& usemtlsInMesh, FILE* file, std::vector<unsigned int>& vertexIndices, std::vector<unsigned int>& uvIndices, std::vector<unsigned int>& normalIndices,
					   std::vector<glm::vec3>& temp_vertices, std::vector<glm::vec2>& temp_uvs, std::vector<glm::vec3>& temp_normals, std::vector<glm::vec3>& temp_tangents,
					   std::vector<glm::vec3>& temp_bitangents);
	bool LoadModelBinary(const char* binaryFile);
	bool LoadModelGLTF(const char* binaryFile, float verticesScalar);
	void PassDataToMesh(std::vector<unsigned int>& vertexIndices, std::vector<unsigned int>& uvIndices, std::vector<unsigned int>& normalIndices, class obj_mesh* activeMesh,
						std::vector<glm::vec3>& temp_vertices, std::vector<glm::vec2>& temp_uvs, std::vector<glm::vec3>& temp_normals, std::vector<glm::vec3>& temp_tangents,
						std::vector<glm::vec3>& temp_bitangents);
};