#include "fps_camera.h"

#include "ecs/coordinator.hpp"
#include "engine.h"
#include "glad/glad.h"
#include "glfw/glfw3.h"

extern Coordinator gCoordinator;
void fps_camera_system::Update(float& deltaTime)
{
	if (firstIteration)
	{
		lastX = engine::Instance()->windowMouseX / (float)engine::Instance()->windowInfo.windowWidth;
		lastY = engine::Instance()->windowMouseY / (float)engine::Instance()->windowInfo.windowHeight;
		firstIteration = false;
	}
	float offsetX = engine::Instance()->windowMouseX / (float)engine::Instance()->windowInfo.windowWidth - lastX;
	float offsetY = lastY - engine::Instance()->windowMouseY / (float)engine::Instance()->windowInfo.windowHeight;

	lastX = engine::Instance()->windowMouseX / (float)engine::Instance()->windowInfo.windowWidth;
	lastY = engine::Instance()->windowMouseY / (float)engine::Instance()->windowInfo.windowHeight;

	for (const auto& entity : mEntities)
	{
		auto& fps = gCoordinator.GetComponent<fps_camera>(entity);
		float pitch;
		float yaw;
		float roll;
		fps.cam->GetRotation(pitch, yaw, roll);
		pitch += offsetY * fps.movementSpeed;
		yaw += offsetX * fps.movementSpeed;
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		fps.cam->SetRotation(pitch, yaw, 0);
	}
}
