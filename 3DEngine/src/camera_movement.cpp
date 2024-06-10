#include "camera_movement.h"

#include "ecs/Coordinator.hpp"
#include "engine.h"

extern Coordinator gCoordinator;
void camera_movement_system::Update(float& deltaTime)
{
	for (const auto& entity : mEntities)
	{
		auto& movement = gCoordinator.GetComponent<camera_movement>(entity);
		for (auto& key : movement.keyToDirection)
		{
			int inputKey = key.first;
			if (engine::Instance()->keyHolder.KeyDown(inputKey))
			{
				glm::vec3 dir;
				switch (key.second)
				{
				case FORWARD:
					dir = movement.cam->front;
					break;
				case BACKWARD:
					dir = movement.cam->front * -1.0f;
					break;
				case RIGHT:
					dir = movement.cam->right;
					break;
				case LEFT:
					dir = movement.cam->right * -1.0f;
					break;
				case UP:
					dir = movement.cam->up;
					break;
				case DOWN:
					dir = movement.cam->up * -1.0f;
					break;
				default:
					break;
				}
				movement.cam->position = movement.cam->position + dir * movement.speed * deltaTime;
			}
		}
	}
}