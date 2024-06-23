#include "frustum.h"

#include "camera.h"
#include "model.h"

frustum CreateFrustumFromCamera(const camera* cam)
{
	frustum frustum;
	const float halfVSide = cam->farPlane * tanf(glm::radians(cam->fov) * .5f);
	const float halfHSide = halfVSide * cam->aspectRatio;
	const glm::vec3 frontMultFar = cam->farPlane * cam->front;

	frustum.nearFace = {(glm::vec3)cam->position + cam->nearPlane * cam->front, cam->front};
	frustum.farFace = {(glm::vec3)cam->position + frontMultFar, -cam->front};
	frustum.rightFace = {(glm::vec3)cam->position, glm::cross(frontMultFar - cam->right * halfHSide, cam->up)};
	frustum.leftFace = {(glm::vec3)cam->position, glm::cross(cam->up, frontMultFar + cam->right * halfHSide)};
	frustum.bottomFace = {(glm::vec3)cam->position, glm::cross(cam->right, frontMultFar - cam->up * halfVSide)};
	frustum.topFace = {(glm::vec3)cam->position, glm::cross(frontMultFar + cam->up * halfVSide, cam->right)};

	return frustum;
}

aabb GenerateAABB(const model* m)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	for (auto& mesh : m->meshes)
	{
		for (int i = 0; i < mesh->vertexCount; i++)
		{
			minAABB.x = std::min(minAABB.x, mesh->vertices[i].position.x);
			minAABB.y = std::min(minAABB.y, mesh->vertices[i].position.y);
			minAABB.z = std::min(minAABB.z, mesh->vertices[i].position.z);

			maxAABB.x = std::max(maxAABB.x, mesh->vertices[i].position.x);
			maxAABB.y = std::max(maxAABB.y, mesh->vertices[i].position.y);
			maxAABB.z = std::max(maxAABB.z, mesh->vertices[i].position.z);
		}
	}
	return aabb(minAABB, maxAABB);
}

sphere GenerateSphereBV(const model* m)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
	for (auto& mesh : m->meshes)
	{
		for (int i = 0; i < mesh->vertexCount; i++)
		{
			minAABB.x = std::min(minAABB.x, mesh->vertices[i].position.x);
			minAABB.y = std::min(minAABB.y, mesh->vertices[i].position.y);
			minAABB.z = std::min(minAABB.z, mesh->vertices[i].position.z);

			maxAABB.x = std::max(maxAABB.x, mesh->vertices[i].position.x);
			maxAABB.y = std::max(maxAABB.y, mesh->vertices[i].position.y);
			maxAABB.z = std::max(maxAABB.z, mesh->vertices[i].position.z);
		}
	}

	return sphere((maxAABB + minAABB) * 0.5f, glm::length(minAABB - maxAABB));
}

aabb GenerateAABB(const mesh* m)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	for (int i = 0; i < m->vertexCount; i++)
	{
		minAABB.x = std::min(minAABB.x, m->vertices[i].position.x);
		minAABB.y = std::min(minAABB.y, m->vertices[i].position.y);
		minAABB.z = std::min(minAABB.z, m->vertices[i].position.z);

		maxAABB.x = std::max(maxAABB.x, m->vertices[i].position.x);
		maxAABB.y = std::max(maxAABB.y, m->vertices[i].position.y);
		maxAABB.z = std::max(maxAABB.z, m->vertices[i].position.z);
	}
	return aabb(minAABB, maxAABB);
}

sphere GenerateSphereBV(const mesh* m)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	for (int i = 0; i < m->vertexCount; i++)
	{
		minAABB.x = std::min(minAABB.x, m->vertices[i].position.x);
		minAABB.y = std::min(minAABB.y, m->vertices[i].position.y);
		minAABB.z = std::min(minAABB.z, m->vertices[i].position.z);

		maxAABB.x = std::max(maxAABB.x, m->vertices[i].position.x);
		maxAABB.y = std::max(maxAABB.y, m->vertices[i].position.y);
		maxAABB.z = std::max(maxAABB.z, m->vertices[i].position.z);
	}
	return sphere((maxAABB + minAABB) * 0.5f, glm::length(minAABB - maxAABB));
}
