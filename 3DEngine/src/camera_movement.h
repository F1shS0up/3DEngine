#pragma once
#include "camera.h"
#include "ecs/System.hpp"
#include "glfw/glfw3.h"
#include "glm/glm.hpp"

#include <map>

enum direction
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	FORWARD,
	BACKWARD
};
struct camera_movement
{
	camera* cam;
	std::vector<std::pair<int, direction>> keyToDirection = {{GLFW_KEY_W, FORWARD}, {GLFW_KEY_S, BACKWARD}, {GLFW_KEY_A, LEFT}, {GLFW_KEY_D, RIGHT}, {GLFW_KEY_Q, DOWN}, {GLFW_KEY_E, UP}};
	float speed = 10.0f;
};

struct camera_movement_system : public System
{
	void Update(float& deltaTime);
};
