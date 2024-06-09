/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "resource_manager.h"

#include "stb_image.h"

#include <fstream>
#include <iostream>
#include <sstream>

// Instantiate static variables
std::map<std::string, texture2D> resource_manager::textures;
std::map<std::string, shader> resource_manager::shaders;
std::map<std::string, mesh> resource_manager::meshes;
std::map<std::string, unsigned int> resource_manager::cubemaps;

shader* resource_manager::LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name)
{
	if (shaders.find(name) != shaders.end()) std::cout << "Shader already exists: " << name << std::endl;
	shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	return &shaders[name];
}

shader* resource_manager::GetShader(std::string name)
{
	if (shaders.find(name) == shaders.end()) std::cout << "Shader not found: " << name << std::endl;
	return &shaders[name];
}

texture2D* resource_manager::LoadTexture(const char* file, std::string name, bool repeat)
{
	if (textures.find(name) != textures.end()) std::cout << "Texture already exists: " << name << std::endl;
	textures[name] = loadTextureFromFile(file, repeat);
	return &textures[name];
}

texture2D* resource_manager::GetTexture(std::string name)
{
	if (textures.find(name) == textures.end()) std::cout << "Texture not found: " << name << std::endl;
	return &textures[name];
}

mesh* resource_manager::LoadMesh(const char* file, std::string name)
{
	if (meshes.find(name) != meshes.end()) std::cout << "Mesh already exists: " << name << std::endl;
	meshes[name] = mesh(file);
	return &meshes[name];
}

mesh* resource_manager::GetMesh(std::string name)
{
	if (meshes.find(name) == meshes.end()) std::cout << "Mesh not found: " << name << std::endl;
	return &meshes[name];
}

unsigned int resource_manager::LoadCubemap(std::string folder, std::string extension, std::string name)
{
	if (cubemaps.find(name) != cubemaps.end()) std::cout << "Cubemap already exists: " << name << std::endl;
	cubemaps[name] = loadCubemap(std::vector<std::string> {folder + "/right" + extension, folder + "/left" + extension, folder + "/top" + extension, folder + "/bottom" + extension,
														   folder + "/front" + extension, folder + "/back" + extension});
	return cubemaps[name];
}

unsigned int resource_manager::LoadCubemap(std::vector<std::string> faces, std::string name)
{
	if (cubemaps.find(name) != cubemaps.end()) std::cout << "Cubemap already exists: " << name << std::endl;
	cubemaps[name] = loadCubemap(faces);
	return cubemaps[name];
}

unsigned int resource_manager::GetCubemap(std::string name)
{
	if (cubemaps.find(name) == cubemaps.end()) std::cout << "Mesh not found: " << name << std::endl;
	return cubemaps[name];
}

void resource_manager::Clear()
{
	// (properly) delete all shaders
	for (auto iter : shaders)
		glDeleteProgram(iter.second.ID);
	// (properly) delete all textures
	for (auto iter : textures)
		glDeleteTextures(1, &iter.second.ID);
}

shader resource_manager::loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	try
	{
		// open files
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);
		std::stringstream vshaderstream, fshaderstream;
		// read file's buffer contents into streams
		vshaderstream << vertexShaderFile.rdbuf();
		fshaderstream << fragmentShaderFile.rdbuf();
		// close file handlers
		vertexShaderFile.close();
		fragmentShaderFile.close();
		// convert stream into string
		vertexCode = vshaderstream.str();
		fragmentCode = fshaderstream.str();
		// if geometry shader path is present, also load a geometry shader
		if (gShaderFile != nullptr)
		{
			std::ifstream geometryShaderFile(gShaderFile);
			std::stringstream gshaderstream;
			gshaderstream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gshaderstream.str();
		}
	}
	catch (std::exception e)
	{
		std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	const char* gShaderCode = geometryCode.c_str();
	// 2. now create shader object from source code
	shader shader;
	shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
	return shader;
}

texture2D resource_manager::loadTextureFromFile(const char* file, bool repeat)
{
	// create texture object
	texture2D texture;
	if (repeat)
	{
		texture.Wrap_S = GL_REPEAT;
		texture.Wrap_T = GL_REPEAT;
	}
	// load image
	stbi_set_flip_vertically_on_load(false);
	int width, height, nrChannels;
	unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
	if (nrChannels == 4)
	{
		texture.Internal_Format = GL_RGBA;
		texture.Image_Format = GL_RGBA;
	}
	else if (nrChannels == 3)
	{
		texture.Internal_Format = GL_RGB;
		texture.Image_Format = GL_RGB;
	}
	else
	{
		texture.Internal_Format = GL_RED;
		texture.Image_Format = GL_RED;
	}
	// now generate texture
	texture.Generate(width, height, data);
	// and finally free image data
	stbi_image_free(data);
	return texture;
}

unsigned int resource_manager::loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}