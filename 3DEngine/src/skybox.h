#pragma once
#include "ecs/System.hpp"
const float skyboxVertices[] = {
	// positions
	-1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,	 -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

	-1.0f, -1.0f, 1.0f,	 -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,	-1.0f, -1.0f, 1.0f,	 -1.0f, -1.0f, 1.0f,  1.0f,	 -1.0f, -1.0f, 1.0f,

	1.0f,  -1.0f, -1.0f, 1.0f,	-1.0f, 1.0f,  1.0f,	 1.0f,	1.0f,  1.0f,  1.0f,	 1.0f,	1.0f,  1.0f,  -1.0f, 1.0f,	-1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,	 -1.0f, 1.0f,  1.0f,  1.0f,	 1.0f,	1.0f,  1.0f,  1.0f,	 1.0f,	1.0f,  -1.0f, 1.0f,	 -1.0f, -1.0f, 1.0f,

	-1.0f, 1.0f,  -1.0f, 1.0f,	1.0f,  -1.0f, 1.0f,	 1.0f,	1.0f,  1.0f,  1.0f,	 1.0f,	-1.0f, 1.0f,  1.0f,	 -1.0f, 1.0f,  -1.0f,

	-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,	 -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,	 1.0f,	-1.0f, 1.0f};
struct skybox
{
	unsigned int cubemap;

	unsigned int VAO;
};

struct skybox_system : public System
{
	void Init();
	void Render();
};
