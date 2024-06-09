#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "mesh.h"
#include "shader.h"
#include "texture.h"

#include <glad/glad.h>
#include <map>
#include <string>

// A static singleton resource_manager class that hosts several
// functions to load Textures and Shaders. Each loaded texture
// and/or shader is also stored for future reference by string
// handles. All functions and resources are static and no
// public constructor is defined.
class resource_manager
{
public:
	// resource storage
	static std::map<std::string, shader> shaders;
	static std::map<std::string, texture2D> textures;
	static std::map<std::string, unsigned int> cubemaps;
	static std::map<std::string, mesh> meshes;
	// loads (and generates) a shader program from file loading vertex, fragment (and geometry)
	// shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
	static shader* LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);
	// retrieves a stored sader
	static shader* GetShader(std::string name);
	// loads (and generates) a texture from file
	static texture2D* LoadTexture(const char* file, std::string name, bool repeat = false);
	// retrieves a stored texture
	static texture2D* GetTexture(std::string name);

	static mesh* LoadMesh(const char* file, std::string name);
	static mesh* GetMesh(std::string name);

	static unsigned int LoadCubemap(std::string folder, std::string extension, std::string name);
	static unsigned int LoadCubemap(std::vector<std::string> faces, std::string name);
	static unsigned int GetCubemap(std::string name);

	// properly de-allocates all loaded resources
	static void Clear();

private:
	// private constructor, that is we do not want any actual resource manager objects. Its members
	// and functions should be publicly available (static).
	resource_manager() { }
	// loads and generates a shader from file
	static shader loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);
	// loads a single texture from file
	static texture2D loadTextureFromFile(const char* file, bool repeat = false);

	static unsigned int loadCubemap(std::vector<std::string> faces);
};

#endif