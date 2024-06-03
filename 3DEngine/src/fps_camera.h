#pragma once
#include "camera.h"
#include "ecs/System.hpp"

struct fps_camera
{
	camera* cam;
	float movementSpeed = 100.f;
};

struct fps_camera_system : public System
{
	float lastX = 0, lastY = 0;
	bool firstIteration = true;
	void Update(float& deltaTime);
};