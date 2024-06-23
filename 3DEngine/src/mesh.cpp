#include "mesh.h"

#include "shader.h"

void mesh::Init()
{
	bv = new aabb(GenerateAABB(this));
	std::cout << "Bounding box created" << std::endl;
}

void mesh::Render(shader* s) { }
