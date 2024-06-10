#include "mesh.h"

#include "glad/glad.h"
#include "shader.h"

#include <iostream>
#include <string>

bool mesh::LoadOBJ(const char* path, std::vector<vertex>& vertices)
{
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;
	std::vector<glm::vec3> temp_tangents;
	std::vector<glm::vec3> temp_bitangents;
	FILE* file = fopen(path, "r");
	if (file == NULL)
	{
		printf("Impossible to open the file !\n");
		return false;
	}

	while (1)
	{

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
			temp_tangents.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
			temp_bitangents.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2],
								 &normalIndex[2]);
			if (matches != 9)
			{
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);

			// Calculate Tangents and Bitangents
			glm::vec3& v0 = temp_vertices[vertexIndex[0] - 1];
			glm::vec3& v1 = temp_vertices[vertexIndex[1] - 1];
			glm::vec3& v2 = temp_vertices[vertexIndex[2] - 1];

			// Shortcuts for UVs
			glm::vec2& uv0 = temp_uvs[uvIndex[0] - 1];
			glm::vec2& uv1 = temp_uvs[uvIndex[1] - 1];
			glm::vec2& uv2 = temp_uvs[uvIndex[2] - 1];

			// Edges of the triangle : position delta
			glm::vec3 deltaPos1 = v1 - v0;
			glm::vec3 deltaPos2 = v2 - v0;

			// UV delta
			glm::vec2 deltaUV1 = uv1 - uv0;
			glm::vec2 deltaUV2 = uv2 - uv0;

			if (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x == 0.0f)
			{
				std::cout << "Degenerate Triangle" << std::endl;
			}

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
			glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

			temp_tangents[vertexIndex[0] - 1] += tangent;
			temp_tangents[vertexIndex[1] - 1] += tangent;
			temp_tangents[vertexIndex[2] - 1] += tangent;
			temp_bitangents[vertexIndex[0] - 1] += bitangent;
			temp_bitangents[vertexIndex[1] - 1] += bitangent;
			temp_bitangents[vertexIndex[2] - 1] += bitangent;
		}
	}
	int verticesCount = vertexIndices.size();
	for (unsigned int i = 0; i < verticesCount; i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];
		vertices.push_back(vertex(temp_vertices[vertexIndex - 1], temp_uvs[uvIndex - 1], temp_normals[normalIndex - 1], temp_tangents[vertexIndex - 1], temp_bitangents[vertexIndex - 1]));
	}

	vol = new aabb(GenerateAABB(this));
}

void mesh::InitRenderData()
{
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
	glEnableVertexAttribArray(4);
}

void mesh::Render(shader* s)
{
	s->Use();
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);
}
