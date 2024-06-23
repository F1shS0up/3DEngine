#pragma once
#include "frustum.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#include <iostream>
#include <vector>

struct camera
{
private:
	float pitch, yaw;
	glm::vec3 worldUp;

public:
	frustum cameraFrustum;
	glm::vec4 position;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	glm::mat4 projection;
	glm::mat4 view;

	float nearPlane, farPlane;
	float fov;
	float aspectRatio;
	int outputWidth, outputHeight;
	int fboTargetOffsetX, fboTargetOffsetY;
	int fboTargetWidth, fboTargetHeight;

	camera(int outputWidth, int outputHeight, float nearPlane, float farPlane, float fov, glm::vec4 position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), float pitch = 0, float yaw = 0,
		   glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f));

	void ComputeViewMatrix()
	{
		view = glm::lookAt(glm::vec3(position), glm::vec3(position) + front, up);
	}

	void SetRotation(float pitch, float yaw, float roll)
	{
		this->pitch = pitch;
		this->yaw = yaw;
		UpdateCameraVectors();
	}

	void GetRotation(float& pitch, float& yaw, float& roll)
	{
		pitch = this->pitch;
		yaw = this->yaw;
		roll = 0;
	}

	void SetPitch(float pitch)
	{
		this->pitch = pitch;
		UpdateCameraVectors();
	}
	void SetYaw(float yaw)
	{
		this->yaw = yaw;
		UpdateCameraVectors();
	}
	void SetRoll(float roll)
	{
		UpdateCameraVectors();
	}

	void Pitch(float angle)
	{
		pitch += angle;
		UpdateCameraVectors();
	}

	void Yaw(float angle)
	{
		yaw += angle;
		UpdateCameraVectors();
	}

	void Roll(float angle)
	{
		UpdateCameraVectors();
	}

private:
	void UpdateCameraVectors();
};

struct camera_system
{
	std::vector<camera> cameras;
	void CreateCamera(camera cam);
	camera* GetCurrentCamera()
	{
		return currentCamera;
	}

private:
	camera* currentCamera = nullptr;
};
