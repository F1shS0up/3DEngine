#include "model.h"

#include "ordinal_suffix.h"

#include "json/json.h"
#include <fstream>
#include <string>

bool model::LoadModelObj(const char* path, float verticesScalar)
{
	std::vector<unsigned int> indices;
	std::vector<obj_vertex> vertices;
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
	mesh* lastMesh;
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
				PassDataToMesh(indices, meshes[meshes.size() - 1], vertices);
				meshes[meshes.size() - 1]->Init();
			}
			meshes.push_back(new mesh());
			meshes[meshes.size() - 1]->name = name;
			lastMesh = meshes[meshes.size() - 1];

			indices.clear();
		}
		else if (strcmp(lineHeader, "usemtl") == 0)
		{
			if (usemtlsInMesh > 0)
			{
				std::string name = lastMesh->name + std::to_string(usemtlsInMesh);
				// New object
				if (meshes.size() > 0)
				{
					PassDataToMesh(indices, meshes[meshes.size() - 1], vertices);
					meshes[meshes.size() - 1]->Init();
				}
				meshes.push_back(new mesh());
				meshes[meshes.size() - 1]->name = name;
			}

			int materialIndex;
			int scan = fscanf(file, "%d", &materialIndex);
			if (scan == 1)
			{
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

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
			glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

			temp_tangents[vertexIndex[0] - 1] += tangent;
			temp_tangents[vertexIndex[1] - 1] += tangent;
			temp_tangents[vertexIndex[2] - 1] += tangent;
			temp_bitangents[vertexIndex[0] - 1] += bitangent;
			temp_bitangents[vertexIndex[1] - 1] += bitangent;
			temp_bitangents[vertexIndex[2] - 1] += bitangent;

			auto i0 = std::find(vertices.begin(), vertices.end(), obj_vertex {vertexIndex[0], uvIndex[0], normalIndex[0]});
			int realIndex0 = i0 - vertices.begin();
			if (i0 == vertices.end())
			{
				vertices.push_back(obj_vertex {vertexIndex[0], uvIndex[0], normalIndex[0], temp_vertices[vertexIndex[0] - 1], temp_uvs[uvIndex[0] - 1], temp_normals[normalIndex[0] - 1],
											   temp_tangents[vertexIndex[0] - 1], temp_bitangents[vertexIndex[0] - 1]});
			}
			indices.push_back(realIndex0);

			auto i1 = std::find(vertices.begin(), vertices.end(), obj_vertex {vertexIndex[1], uvIndex[1], normalIndex[1]});
			int realIndex1 = i1 - vertices.begin();
			if (i1 == vertices.end())
			{
				vertices.push_back(obj_vertex {vertexIndex[1], uvIndex[1], normalIndex[1], temp_vertices[vertexIndex[1] - 1], temp_uvs[uvIndex[1] - 1], temp_normals[normalIndex[1] - 1],
											   temp_tangents[vertexIndex[1] - 1], temp_bitangents[vertexIndex[1] - 1]});
			}
			indices.push_back(realIndex1);

			auto i2 = std::find(vertices.begin(), vertices.end(), obj_vertex {vertexIndex[2], uvIndex[2], normalIndex[2]});
			int realIndex2 = i2 - vertices.begin();
			if (i2 == vertices.end())
			{
				vertices.push_back(obj_vertex {vertexIndex[2], uvIndex[2], normalIndex[2], temp_vertices[vertexIndex[2] - 1], temp_uvs[uvIndex[2] - 1], temp_normals[normalIndex[2] - 1],
											   temp_tangents[vertexIndex[2] - 1], temp_bitangents[vertexIndex[2] - 1]});
			}
			indices.push_back(realIndex2);
		}
	}
	PassDataToMesh(indices, meshes[meshes.size() - 1], vertices);
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
	int* meshIndicesCounts = new int[numberOfMeshes];
	for (int i = 0; i < numberOfMeshes; i++)
	{
		myfile.read((char*)&meshVertexCounts[i], sizeof(int));
		myfile.read((char*)&meshIndicesCounts[i], sizeof(int));
	}
	meshes.reserve(numberOfMeshes);
	for (int i = 0; i < numberOfMeshes; i++)
	{
		mesh* m = new mesh();
		m->vertices = new vertex[meshVertexCounts[i]];
		m->vertexCount = meshVertexCounts[i];
		m->indices = new unsigned int[meshIndicesCounts[i]];
		m->indexCount = meshIndicesCounts[i];
		char name[40];
		myfile.read((char*)&name[0], sizeof(char) * 40);
		m->name = name;
		myfile.read((char*)&m->vertices[0], sizeof(vertex) * meshVertexCounts[i]);
		std::cout << meshIndicesCounts[i] << std::endl;
		myfile.read((char*)&m->indices[0], sizeof(unsigned int) * meshIndicesCounts[i]);
		myfile.read((char*)&m->averagePosition, sizeof(glm::vec4));
		myfile.read((char*)&m->materialIndex, sizeof(int));
		m->Init();

		meshes.push_back(m);
	}
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
	if (!reader.parse(jsonStr, json))
	{
		std::cout << "Problem parsing assetData: " + jsonStr << std::endl;
		return false;
	}

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
	Json::Value& nodes = json["nodes"];

	int nodeCount = meshesJ.size();
	for (int i = 0; i < nodeCount; i++)
	{
		int meshIndex = nodes[i]["mesh"].asInt();
		Json::Value& meshJ = meshesJ[meshIndex];
		int primitiveCount = meshJ["primitives"].size();

		for (int j = 0; j < primitiveCount; j++)
		{
			Json::Value& primitive = meshJ["primitives"][j];

			bool hasMaterial = primitive.isMember("material");
			int mat;
			if (hasMaterial)
			{
				mat = primitive["material"].asInt();
			}
			else
			{
				mat = 0;
			}

			Json::Value& attributes = primitive["attributes"];

			bool useTangent = attributes.isMember("TANGENT");

			Json::Value& indicesAccessor = accessors[primitive["indices"].asInt()];
			Json::Value& positionAccessor = accessors[attributes["POSITION"].asInt()];
			Json::Value& normalAccessor = accessors[attributes["NORMAL"].asInt()];
			Json::Value& uvAccessor = accessors[attributes["TEXCOORD_0"].asInt()];

			Json::Value& indicesBufferView = bufferViews[indicesAccessor["bufferView"].asInt()];
			Json::Value& positionalBufferView = bufferViews[positionAccessor["bufferView"].asInt()];
			Json::Value& normalBufferView = bufferViews[normalAccessor["bufferView"].asInt()];
			Json::Value& uvBufferView = bufferViews[uvAccessor["bufferView"].asInt()];

			int indicesByteOffset = indicesBufferView["byteOffset"].asInt();
			int positionalByteOffset = positionalBufferView["byteOffset"].asInt();
			int normalByteOffset = normalBufferView["byteOffset"].asInt();
			int uvByteOffset = uvBufferView["byteOffset"].asInt();

			int iCount = indicesAccessor["count"].asInt();
			int pCount = positionAccessor["count"].asInt();

			int indicesByteLength = indicesBufferView["byteLength"].asInt();
			int positionalByteLength = positionalBufferView["byteLength"].asInt();
			int normalByteLength = normalBufferView["byteLength"].asInt();
			int uvByteLength = uvBufferView["byteLength"].asInt();

			int verticesCount = positionAccessor["count"].asInt();
			int indexCount = indicesAccessor["count"].asInt();

			int32_t* indices = new int32_t[indexCount];
			glm::vec3* temp_vertices = new glm::vec3[verticesCount];
			glm::vec2* temp_uvs = new glm::vec2[verticesCount];
			glm::vec3* temp_normals = new glm::vec3[verticesCount];
			glm::vec4* temp_tangents = new glm::vec4[verticesCount];

			if (indicesByteLength / iCount == 2)
			{
				int16_t* indicesData = (int16_t*)(bin.data() + indicesByteOffset);
				std::copy(indicesData, indicesData + iCount, &indices[0]);
			}
			else
			{
				int32_t* indicesData = (int32_t*)(bin.data() + indicesByteOffset);
				std::copy(indicesData, indicesData + iCount, &indices[0]);
			}

			float* positionData = (float*)(bin.data() + positionalByteOffset);
			memcpy(&temp_vertices[0], positionData, positionalByteLength);

			float* normalData = (float*)(bin.data() + normalByteOffset);
			memcpy(&temp_normals[0], normalData, normalByteLength);

			float* uvData = (float*)(bin.data() + uvByteOffset);
			memcpy(&temp_uvs[0], uvData, uvByteLength);

			if (useTangent)
			{
				int tangentAccessorInt = attributes["TANGENT"].asInt();
				Json::Value& tangentAccessor = accessors[tangentAccessorInt];
				Json::Value& tangentBufferView = bufferViews[tangentAccessor["bufferView"].asInt()];
				int tangentByteOffset = tangentBufferView["byteOffset"].asInt();
				int tangentByteLength = tangentBufferView["byteLength"].asInt();
				float* tangentData = (float*)(bin.data() + tangentByteOffset);
				memcpy(&temp_tangents[0], tangentData, tangentByteLength);
			}

			mesh* activeMesh = new mesh();
			activeMesh->materialIndex = mat;
			activeMesh->name = meshJ["name"].asString();
			activeMesh->indexCount = indexCount;
			activeMesh->indices = new unsigned int[indexCount];
			std::copy(indices, indices + indexCount, activeMesh->indices);

			activeMesh->vertexCount = verticesCount;
			activeMesh->vertices = new vertex[verticesCount];
			for (unsigned int x = 0; x < verticesCount; x++)
			{
				glm::vec3 bitangent = glm::cross(temp_normals[x], (glm::vec3)temp_tangents[x]);
				activeMesh->vertices[x] = vertex(temp_vertices[x] * verticesScalar, temp_uvs[x], temp_normals[x], temp_tangents[x], bitangent);
			}

			delete[] temp_vertices;
			delete[] temp_uvs;
			delete[] temp_normals;
			delete[] temp_tangents;
			delete[] indices;

			activeMesh->Init();
			meshes.push_back(activeMesh);
		}
	}
	return true;
}

void model::PassDataToMesh(std::vector<unsigned int>& indices, mesh* activeMesh, std::vector<obj_vertex>& objVertices)
{
	int verticesCount = objVertices.size();
	activeMesh->vertices = new vertex[verticesCount];
	activeMesh->vertexCount = verticesCount;
	for (unsigned int i = 0; i < verticesCount; i++)
	{
		activeMesh->vertices[i] = vertex(objVertices[i].position, objVertices[i].uv, objVertices[i].normal, objVertices[i].tangent, objVertices[i].bitangent);
		activeMesh->averagePosition += glm::vec4(objVertices[i].position, 1.0f);
	}

	int indexCount = indices.size();
	activeMesh->indices = new unsigned int[indexCount];
	activeMesh->indexCount = indexCount;
	std::copy(indices.begin(), indices.end(), activeMesh->indices);
	activeMesh->averagePosition = activeMesh->averagePosition / (float)verticesCount;
}
