#include "skybox.h"

#include "ecs/coordinator.hpp"
#include "glad/glad.h"
#include "resource_manager.h"

extern Coordinator gCoordinator;

void skybox_system::Init()
{
	for (const auto& entity : mEntities)
	{
		auto& s = gCoordinator.GetComponent<skybox>(entity);
		unsigned int VBO;
		glGenVertexArrays(1, &s.VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(s.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}
}

void skybox_system::Render()
{
	glDepthMask(GL_FALSE);
	resource_manager::GetShader("skybox")->Use();
	for (const auto& entity : mEntities)
	{
		auto& s = gCoordinator.GetComponent<skybox>(entity);
		glBindVertexArray(s.VAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, s.cubemap);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glDepthMask(GL_TRUE);
}