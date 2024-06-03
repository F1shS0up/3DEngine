#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#include <iostream>
#include <vector>

struct camera
{
private:
	glm::vec3 position;
	float pitch, yaw;
	glm::vec3 worldUp;

public:
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	glm::mat4 projection;

	float nearPlane, farPlane;
	float fov;
	int outputWidth, outputHeight;
	int fboTargetOffsetX, fboTargetOffsetY;
	int fboTargetWidth, fboTargetHeight;

	camera(int outputWidth, int outputHeight, float nearPlane, float farPlane, float fov, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float pitch = 0, float yaw = 0,
		   glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(position, position + front, up);
	}

	void SetPosition(glm::vec3 position)
	{
		this->position = position;
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

	glm::vec3 GetPosition()
	{
		return position;
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
