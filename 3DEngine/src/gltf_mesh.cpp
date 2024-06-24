#include "gltf_mesh.h"

#include "glad/glad.h"
#include "shader.h"

#include <iostream>
#include <string>

void gltf_mesh::Init()
{
	mesh::Init();
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertexCount, &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicesCount, &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float) + sizeof(int), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float) + sizeof(int), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float) + sizeof(int), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float) + sizeof(int), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float) + sizeof(int), (void*)(11 * sizeof(float)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 1, GL_INT, GL_FALSE, 14 * sizeof(float) + sizeof(int), (void*)(14 * sizeof(float)));
	glEnableVertexAttribArray(5);
}

void gltf_mesh::Render(shader* s)
{
	s->Use();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
