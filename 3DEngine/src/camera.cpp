#include "camera.h"

#include "engine.h"

#include <iostream>

void camera_system::CreateCamera(camera cam)
{
	cameras.push_back(cam);
	if (cameras.size() == 1)
	{
		currentCamera = &cameras[0];
	}
}

camera::camera(int outputWidth, int outputHeight, float nearPlane, float farPlane, float fov, glm::vec4 position, float pitch, float yaw, glm::vec3 worldUp) :
	position(position), pitch(pitch), yaw(yaw), worldUp(worldUp), outputWidth(outputWidth), outputHeight(outputHeight), nearPlane(nearPlane), farPlane(farPlane), fov(fov)
{
	if (outputWidth == 0 || outputHeight == 0)
	{
		this->outputWidth = engine::Instance()->windowInfo.windowWidth;
		this->outputHeight = engine::Instance()->windowInfo.windowHeight;
		std::cout << "Camera output size not specified, using window size: " << this->outputWidth << "x" << this->outputHeight << std::endl;
	}
	projection = glm::perspective(glm::radians(fov), (float)this->outputWidth / (float)this->outputHeight, nearPlane, farPlane);
	aspectRatio = (float)this->outputWidth / (float)this->outputHeight;
	UpdateCameraVectors();
}

void camera::UpdateCameraVectors()
{
	cameraFrustum = CreateFrustumFromCamera(this);
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	// also re-calculate the Right and Up vector
	right = glm::normalize(glm::cross(front, worldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize(glm::cross(right, front));
}
