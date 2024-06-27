#pragma once
#include "ecs/System.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

struct transform
{
	// Local space information
	glm::vec4 pos = {0.0f, 0.0f, 0.0f, 1.0f};
	glm::vec3 eulerRot = {0.0f, 0.0f, 0.0f}; // In degrees
	glm::vec3 scale = {1.0f, 1.0f, 1.0f};

	// Global space information concatenate in matrix
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	class transform* parent = nullptr;

protected:
	glm::mat4 GetLocalModelMatrix()
	{
		const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(eulerRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
		const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(eulerRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
		const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(eulerRot.z), glm::vec3(0.0f, 0.0f, 1.0f));

		// Y * X * Z
		const glm::mat4 rotationMatrix = transformY * transformX * transformZ;

		// translation * rotation * scale (also know as TRS matrix)
		return glm::translate(glm::mat4(1.0f), glm::vec3(pos)) * rotationMatrix * glm::scale(glm::mat4(1.0f), scale);
	}

public:
	void ComputeModelMatrix()
	{
		glm::mat4 parentGlobalModelMatrix = (parent == nullptr) ? glm::mat4(1.0f) : parent->GetModelMatrix();
		modelMatrix = parentGlobalModelMatrix * GetLocalModelMatrix();
	}

	void SetLocalPosition(const glm::vec4& newPosition)
	{
		pos = newPosition;
	}

	void SetLocalRotation(const glm::vec3& newRotation)
	{
		eulerRot = newRotation;
	}

	void SetLocalScale(const glm::vec3& newScale)
	{
		scale = newScale;
	}

	const glm::vec4& GetGlobalPosition() const
	{
		return modelMatrix[3];
	}

	const glm::vec4& GetLocalPosition() const
	{
		return pos;
	}

	const glm::vec3& GetLocalRotation() const
	{
		return eulerRot;
	}

	const glm::vec3& GetLocalScale() const
	{
		return scale;
	}

	const glm::mat4& GetModelMatrix() const
	{
		return modelMatrix;
	}

	glm::vec3 GetRight() const
	{
		return modelMatrix[0];
	}

	glm::vec3 GetUp() const
	{
		return modelMatrix[1];
	}

	glm::vec3 GetBackward() const
	{
		return modelMatrix[2];
	}

	glm::vec3 GetForward() const
	{
		return -modelMatrix[2];
	}

	glm::vec3 GetGlobalScale() const
	{
		return {glm::length(GetRight()), glm::length(GetUp()), glm::length(GetBackward())};
	}
};

struct transform_system : public System
{
	void OnEntityAdded(Entity entity) override;
	void Update();
};