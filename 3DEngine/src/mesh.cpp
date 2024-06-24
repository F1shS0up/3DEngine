#include "mesh.h"

#include "shader.h"

void mesh::Init()
{
	bv = new aabb(GenerateAABB(this));
}

void mesh::Render(shader* s) { }
