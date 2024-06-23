#include "model.h"

#include "gltf_mesh.h"
#include "obj_mesh.h"

#include "json/json.h"
#include <fstream>
#include <string>

bool model::LoadModelObj(const char* path, float verticesScalar)
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

	int usemtlsInMesh = 0;
	obj_mesh* lastOBJMesh;
	while (1)
	{
		char lineHeader[32];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		if (strcmp(lineHeader, "o") == 0)
		{
			usemtlsInMesh = 0; // reset
			char name[40];
			fscanf(file, "%s", name);
			// New object
			if (meshes.size() > 0)
			{
				PassDataToMesh(vertexIndices, uvIndices, normalIndices, (obj_mesh*)meshes[meshes.size() - 1], temp_vertices, temp_uvs, temp_normals, temp_tangents, temp_bitangents);
				meshes[meshes.size() - 1]->Init();
			}
			meshes.push_back(new obj_mesh());
			meshes[meshes.size() - 1]->name = name;
			lastOBJMesh = (obj_mesh*)meshes[meshes.size() - 1];

			vertexIndices.clear();
			uvIndices.clear();
			normalIndices.clear();
		}
		else if (strcmp(lineHeader, "usemtl") == 0)
		{
			if (usemtlsInMesh > 0)
			{
				std::string name = lastOBJMesh->name + std::to_string(usemtlsInMesh);
				std::cout << "Name: " << name << std::endl;
				// New object
				if (meshes.size() > 0)
				{
					PassDataToMesh(vertexIndices, uvIndices, normalIndices, (obj_mesh*)meshes[meshes.size() - 1], temp_vertices, temp_uvs, temp_normals, temp_tangents, temp_bitangents);
					meshes[meshes.size() - 1]->Init();
				}
				meshes.push_back(new obj_mesh());
				meshes[meshes.size() - 1]->name = name;

				vertexIndices.clear();
				uvIndices.clear();
				normalIndices.clear();
			}

			int materialIndex;
			int scan = fscanf(file, "%d", &materialIndex);
			if (scan == 1)
			{
				std::cout << "Material index: " << materialIndex << std::endl;
				meshes[meshes.size() - 1]->materialIndex = materialIndex;
			}
			else
			{
				meshes[meshes.size() - 1]->materialIndex = 0;
			}
			usemtlsInMesh++;
		}
		else if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex * verticesScalar);
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
				printf("Can't calculate Tangents and Bitangents. UVs are too similar.\n");
				continue;
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
	PassDataToMesh(vertexIndices, uvIndices, normalIndices, (obj_mesh*)meshes[meshes.size() - 1], temp_vertices, temp_uvs, temp_normals, temp_tangents, temp_bitangents);
	meshes[meshes.size() - 1]->Init();
}

bool model::LoadModelBinary(const char* binaryFile)
{
	std::ifstream myfile(binaryFile, std::ios::in | std::ios::binary);
	if (!myfile.is_open())
	{
		std::cout << "Failed to open file: " << binaryFile << std::endl;
		return false;
	}
	int numberOfMeshes = 0;

	myfile.read((char*)&numberOfMeshes, sizeof(int));
	int* meshVertexCounts = new int[numberOfMeshes];
	for (int i = 0; i < numberOfMeshes; i++)
	{
		myfile.read((char*)&meshVertexCounts[i], sizeof(int));
	}
	std::cout << "Number of meshes: " << numberOfMeshes << std::endl;
	meshes.reserve(numberOfMeshes);
	for (int i = 0; i < numberOfMeshes; i++)
	{
		obj_mesh* m = new obj_mesh();
		m->vertices = new vertex[meshVertexCounts[i]];
		m->vertexCount = meshVertexCounts[i];
		char name[40];
		myfile.read((char*)&name[0], sizeof(char) * 40);
		m->name = name;
		myfile.read((char*)&m->vertices[0], sizeof(vertex) * meshVertexCounts[i]);
		myfile.read((char*)&m->averagePosition, sizeof(glm::vec4));
		myfile.read((char*)&m->materialIndex, sizeof(int));
		m->Init();

		meshes.push_back(m);
	}
	std::cout << "Meshes Size:" << meshes.size() << std::endl;
}
bool model::LoadModelGLTF(const char* gltfFile, float verticesScalar)
{
	std::ifstream binFile = std::ifstream(gltfFile, std::ios::binary);

	binFile.seekg(12); // Skip past the 12 byte header, to the json header
	uint32_t jsonLength;
	binFile.read((char*)&jsonLength, sizeof(uint32_t)); // Read the length of the json file from it's header

	std::string jsonStr;
	jsonStr.resize(jsonLength);
	binFile.seekg(20);						  // Skip the rest of the JSON header to the start of the string
	binFile.read(jsonStr.data(), jsonLength); // Read out the json string

	// Parse the json
	Json::Reader reader;

	Json::Value json;
	if (!reader.parse(jsonStr, json)) std::cout << "Problem parsing assetData: " + jsonStr << std::endl;

	// After reading from the json, the file cusor will automatically be at the start of the binary header

	uint32_t binLength;
	binFile.read((char*)&binLength, sizeof(binLength));	 // Read out the bin length from it's header
	binFile.seekg(sizeof(uint32_t), std::ios_base::cur); // skip chunk type

	std::vector<char> bin(binLength);
	binFile.read(bin.data(), binLength);
	binFile.close();

	Json::Value& bufferViews = json["bufferViews"];
	Json::Value& accessors = json["accessors"];
	Json::Value& meshesJ = json["meshes"];

	int meshCount = meshesJ.size();
	for (int i = 0; i < meshCount; i++)
	{
		std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
		std::vector<glm::vec3> temp_vertices;
		std::vector<glm::vec2> temp_uvs;
		std::vector<glm::vec3> temp_normals;
		std::vector<glm::vec3> temp_tangents;
		std::vector<glm::vec3> temp_bitangents;

		Json::Value& meshJ = meshesJ[i];
		int primitiveCount = meshJ["primitives"].size();
		for (int j = 0; j < primitiveCount; j++)
		{
			Json::Value& primitive = meshJ["primitives"][j];
			Json::Value& attributes = primitive["attributes"];

			Json::Value& indicesAccessor = accessors[primitive["indices"].asInt()];
			Json::Value& positionAccessor = accessors[attributes["POSITION"].asInt()];
			Json::Value& normalAccessor = accessors[attributes["NORMAL"].asInt()];
			Json::Value& uvAccessor = accessors[attributes["TEXCOORD_0"].asInt()];
			Json::Value& tangentAccessor = accessors[attributes["TANGENT"].asInt()];

			Json::Value& indicesBufferView = bufferViews[indicesAccessor["bufferView"].asInt()];
			Json::Value& positionalBufferView = bufferViews[positionAccessor["bufferView"].asInt()];
			Json::Value& normalBufferView = bufferViews[normalAccessor["bufferView"].asInt()];
			Json::Value& uvBufferView = bufferViews[uvAccessor["bufferView"].asInt()];
			Json::Value& tangentBufferView = bufferViews[tangentAccessor["bufferView"].asInt()];

			int indicesByteOffset = indicesBufferView["byteOffset"].asInt();
			int positionalByteOffset = positionalBufferView["byteOffset"].asInt();
			int normalByteOffset = normalBufferView["byteOffset"].asInt();
			int uvByteOffset = uvBufferView["byteOffset"].asInt();
			int tangentByteOffset = tangentBufferView["byteOffset"].asInt();

			int indicesCount = indicesAccessor["count"].asInt();
			int positionalCount = positionAccessor["count"].asInt();
			int normalCount = normalAccessor["count"].asInt();
			int uvCount = uvAccessor["count"].asInt();
			int tangentCount = tangentAccessor["count"].asInt();

			std::string counts = "Positional Count: " + std::to_string(positionalCount) + "\nNormal Count: " + std::to_string(normalCount) + "\nUV Count: " + std::to_string(uvCount) +
								 "\nTangent Count: " + std::to_string(tangentCount);

			std::cout << counts << std::endl;

			float* indicesData = (float*)(bin.data() + indicesByteOffset);
			for (int k = 0; k < indicesCount; k++)
			{
				vertexIndices.push_back(indicesData[k]);
				std::cout << indicesData[k] << std::endl;
			}

			float* positionData = (float*)(bin.data() + positionalByteOffset);
			for (int k = 0; k < positionalCount; k++)
			{
				glm::vec3 position(positionData[k * 3], positionData[k * 3 + 1], positionData[k * 3 + 2]);
				temp_vertices.push_back(position * verticesScalar);
			}

			float* normalData = (float*)(bin.data() + normalByteOffset);
			for (int k = 0; k < normalCount; k++)
			{
				glm::vec3 normal(normalData[k * 3], normalData[k * 3 + 1], normalData[k * 3 + 2]);
				temp_normals.push_back(normal);
			}

			float* uvData = (float*)(bin.data() + uvByteOffset);
			for (int k = 0; k < uvCount; k++)
			{
				glm::vec2 uv(uvData[k * 2], uvData[k * 2 + 1]);
				temp_uvs.push_back(uv);
			}

			float* tangentData = (float*)(bin.data() + tangentByteOffset);
			for (int k = 0; k < tangentCount; k++)
			{
				glm::vec3 tangent(tangentData[k * 3], tangentData[k * 3 + 1], tangentData[k * 3 + 2]);
				temp_tangents.push_back(tangent);
			}
			// Json::Value& indices = primitive["indices"];
			// LoadMesh(bin.data(), positionAccessor, normalAccessor, uvAccessor, indices, verticesScalar, meshes);
		}
	}
	return true;
}

void model::PassDataToMesh(std::vector<unsigned int>& vertexIndices, std::vector<unsigned int>& uvIndices, std::vector<unsigned int>& normalIndices, obj_mesh* activeMesh,
						   std::vector<glm::vec3>& temp_vertices, std::vector<glm::vec2>& temp_uvs, std::vector<glm::vec3>& temp_normals, std::vector<glm::vec3>& temp_tangents,
						   std::vector<glm::vec3>& temp_bitangents)
{
	int verticesCount = vertexIndices.size();
	activeMesh->vertices = new vertex[verticesCount];
	activeMesh->vertexCount = verticesCount;
	for (unsigned int i = 0; i < verticesCount; i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];
		activeMesh->vertices[i] = vertex(temp_vertices[vertexIndex - 1], temp_uvs[uvIndex - 1], temp_normals[normalIndex - 1], temp_tangents[vertexIndex - 1], temp_bitangents[vertexIndex - 1]);
		activeMesh->averagePosition += glm::vec4(temp_vertices[vertexIndex - 1], 1.0f);
	}
	activeMesh->averagePosition = activeMesh->averagePosition / (float)verticesCount;
}
