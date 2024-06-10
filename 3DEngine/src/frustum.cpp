#include "frustum.h"

#include "camera.h"
#include "mesh.h"

frustum CreateFrustumFromCamera(const camera* cam)
{
	frustum frustum;
	const float halfVSide = cam->farPlane * tanf(glm::radians(cam->fov) * .5f);
	const float halfHSide = halfVSide * cam->aspectRatio;
	const glm::vec3 frontMultFar = cam->farPlane * cam->front;

	frustum.nearFace = {cam->position + cam->nearPlane * cam->front, cam->front};
	frustum.farFace = {cam->position + frontMultFar, -cam->front};
	frustum.rightFace = {cam->position, glm::cross(frontMultFar - cam->right * halfHSide, cam->up)};
	frustum.leftFace = {cam->position, glm::cross(cam->up, frontMultFar + cam->right * halfHSide)};
	frustum.bottomFace = {cam->position, glm::cross(cam->right, frontMultFar - cam->up * halfVSide)};
	frustum.topFace = {cam->position, glm::cross(frontMultFar + cam->up * halfVSide, cam->right)};

	return frustum;
}

aabb GenerateAABB(const mesh* m)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	for (auto&& vertex : m->vertices)
	{
		minAABB.x = std::min(minAABB.x, vertex.position.x);
		minAABB.y = std::min(minAABB.y, vertex.position.y);
		minAABB.z = std::min(minAABB.z, vertex.position.z);

		maxAABB.x = std::max(maxAABB.x, vertex.position.x);
		maxAABB.y = std::max(maxAABB.y, vertex.position.y);
		maxAABB.z = std::max(maxAABB.z, vertex.position.z);
	}
	return aabb(minAABB, maxAABB);
}

sphere GenerateSphereBV(const mesh* m)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	for (auto&& vertex : m->vertices)
	{
		minAABB.x = std::min(minAABB.x, vertex.position.x);
		minAABB.y = std::min(minAABB.y, vertex.position.y);
		minAABB.z = std::min(minAABB.z, vertex.position.z);

		maxAABB.x = std::max(maxAABB.x, vertex.position.x);
		maxAABB.y = std::max(maxAABB.y, vertex.position.y);
		maxAABB.z = std::max(maxAABB.z, vertex.position.z);
	}

	return sphere((maxAABB + minAABB) * 0.5f, glm::length(minAABB - maxAABB));
}
