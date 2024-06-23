#pragma once
#include "glm/glm.hpp"
#include "transform.h"

#include <array>
#include <iostream>
#include <vector>

struct plane
{
	glm::vec3 normal = {0.f, 1.f, 0.f}; // unit vector
	float distance = 0.f;				// Distance with origin

	plane() = default;

	plane(const glm::vec3& p1, const glm::vec3& norm) : normal(glm::normalize(norm)), distance(glm::dot(normal, p1)) { }

	float GetSignedDistanceToPlane(const glm::vec3& point) const
	{
		return glm::dot(normal, point) - distance;
	}
};

struct frustum
{
	plane topFace;
	plane bottomFace;

	plane rightFace;
	plane leftFace;

	plane farFace;
	plane nearFace;
};

struct bounding_volume
{
	virtual bool IsOnFrustum(const frustum& camFrustum, const transform& transform) const = 0;

	virtual bool IsOnOrForwardPlane(const plane& plane) const = 0;

	bool IsOnFrustum(const frustum& camFrustum) const
	{
		return (IsOnOrForwardPlane(camFrustum.leftFace) && IsOnOrForwardPlane(camFrustum.rightFace) && IsOnOrForwardPlane(camFrustum.topFace) && IsOnOrForwardPlane(camFrustum.bottomFace) &&
				IsOnOrForwardPlane(camFrustum.nearFace) && IsOnOrForwardPlane(camFrustum.farFace));
	};
};

struct sphere : public bounding_volume
{
	glm::vec3 center {0.f, 0.f, 0.f};
	float radius {0.f};

	sphere(const glm::vec3& inCenter, float inRadius) : bounding_volume {}, center {inCenter}, radius {inRadius} { }

	bool IsOnOrForwardPlane(const plane& plane) const final
	{
		return plane.GetSignedDistanceToPlane(center) > -radius;
	}

	bool IsOnFrustum(const frustum& camFrustum, const transform& transform) const final
	{
		// Get global scale thanks to our transform
		const glm::vec3 globalScale = transform.GetGlobalScale();

		// Get our global center with process it with the global model matrix of our transform
		const glm::vec3 globalCenter {transform.GetModelMatrix() * glm::vec4(center, 1.f)};

		// To wrap correctly our shape, we need the maximum scale scalar.
		const float maxScale = std::max(std::max(globalScale.x, globalScale.y), globalScale.z);

		// Max scale is assuming for the diameter. So, we need the half to apply it to our radius
		sphere globalSphere(globalCenter, radius * (maxScale * 0.5f));

		// Check Firstly the result that have the most chance to failure to avoid to call all functions.
		return (globalSphere.IsOnOrForwardPlane(camFrustum.leftFace) && globalSphere.IsOnOrForwardPlane(camFrustum.rightFace) && globalSphere.IsOnOrForwardPlane(camFrustum.farFace) &&
				globalSphere.IsOnOrForwardPlane(camFrustum.nearFace) && globalSphere.IsOnOrForwardPlane(camFrustum.topFace) && globalSphere.IsOnOrForwardPlane(camFrustum.bottomFace));
	};
};

struct squareAABB : public bounding_volume
{
	glm::vec3 center {0.f, 0.f, 0.f};
	float extent {0.f};

	squareAABB(const glm::vec3& inCenter, float inExtent) : bounding_volume {}, center {inCenter}, extent {inExtent} { }

	bool IsOnOrForwardPlane(const plane& plane) const final
	{
		// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
		const float r = extent * (std::abs(plane.normal.x) + std::abs(plane.normal.y) + std::abs(plane.normal.z));
		return -r <= plane.GetSignedDistanceToPlane(center);
	}

	bool IsOnFrustum(const frustum& camFrustum, const transform& transform) const final
	{
		// Get global scale thanks to our transform
		const glm::vec3 globalCenter {transform.GetModelMatrix() * glm::vec4(center, 1.f)};

		// Scaled orientation
		const glm::vec3 right = transform.GetRight() * extent;
		const glm::vec3 up = transform.GetUp() * extent;
		const glm::vec3 forward = transform.GetForward() * extent;

		const float newIi = std::abs(glm::dot(glm::vec3 {1.f, 0.f, 0.f}, right)) + std::abs(glm::dot(glm::vec3 {1.f, 0.f, 0.f}, up)) + std::abs(glm::dot(glm::vec3 {1.f, 0.f, 0.f}, forward));

		const float newIj = std::abs(glm::dot(glm::vec3 {0.f, 1.f, 0.f}, right)) + std::abs(glm::dot(glm::vec3 {0.f, 1.f, 0.f}, up)) + std::abs(glm::dot(glm::vec3 {0.f, 1.f, 0.f}, forward));

		const float newIk = std::abs(glm::dot(glm::vec3 {0.f, 0.f, 1.f}, right)) + std::abs(glm::dot(glm::vec3 {0.f, 0.f, 1.f}, up)) + std::abs(glm::dot(glm::vec3 {0.f, 0.f, 1.f}, forward));

		const squareAABB globalAABB(globalCenter, std::max(std::max(newIi, newIj), newIk));

		return (globalAABB.IsOnOrForwardPlane(camFrustum.leftFace) && globalAABB.IsOnOrForwardPlane(camFrustum.rightFace) && globalAABB.IsOnOrForwardPlane(camFrustum.topFace) &&
				globalAABB.IsOnOrForwardPlane(camFrustum.bottomFace) && globalAABB.IsOnOrForwardPlane(camFrustum.nearFace) && globalAABB.IsOnOrForwardPlane(camFrustum.farFace));
	};
};

struct aabb : public bounding_volume
{
	glm::vec3 center {0.f, 0.f, 0.f};
	glm::vec3 extents {0.f, 0.f, 0.f};

	aabb(const glm::vec3& min, const glm::vec3& max) : bounding_volume {}, center {(max + min) * 0.5f}, extents {max.x - center.x, max.y - center.y, max.z - center.z} { }

	aabb(const glm::vec3& inCenter, float iI, float iJ, float iK) : bounding_volume {}, center {inCenter}, extents {iI, iJ, iK} { }

	std::array<glm::vec3, 8> GetVertices() const
	{
		std::array<glm::vec3, 8> vertices;
		vertices[0] = {center.x - extents.x, center.y - extents.y, center.z - extents.z};
		vertices[1] = {center.x + extents.x, center.y - extents.y, center.z - extents.z};
		vertices[2] = {center.x - extents.x, center.y + extents.y, center.z - extents.z};
		vertices[3] = {center.x + extents.x, center.y + extents.y, center.z - extents.z};
		vertices[4] = {center.x - extents.x, center.y - extents.y, center.z + extents.z};
		vertices[5] = {center.x + extents.x, center.y - extents.y, center.z + extents.z};
		vertices[6] = {center.x - extents.x, center.y + extents.y, center.z + extents.z};
		vertices[7] = {center.x + extents.x, center.y + extents.y, center.z + extents.z};
		return vertices;
	}
	// see https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
	bool IsOnOrForwardPlane(const plane& plane) const final
	{
		// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
		const float r = extents.x * std::abs(plane.normal.x) + extents.y * std::abs(plane.normal.y) + extents.z * std::abs(plane.normal.z);

		return -r <= plane.GetSignedDistanceToPlane(center);
	}

	bool IsOnFrustum(const frustum& camFrustum, const transform& transform) const final
	{
		// Get global scale thanks to our transform
		const glm::vec3 globalCenter {transform.GetModelMatrix() * glm::vec4(center, 1.f)};

		// Scaled orientation
		const glm::vec3 right = transform.GetRight() * extents.x;
		const glm::vec3 up = transform.GetUp() * extents.y;
		const glm::vec3 forward = transform.GetForward() * extents.z;

		const float newIi = std::abs(glm::dot(glm::vec3 {1.f, 0.f, 0.f}, right)) + std::abs(glm::dot(glm::vec3 {1.f, 0.f, 0.f}, up)) + std::abs(glm::dot(glm::vec3 {1.f, 0.f, 0.f}, forward));
		const float newIj = std::abs(glm::dot(glm::vec3 {0.f, 1.f, 0.f}, right)) + std::abs(glm::dot(glm::vec3 {0.f, 1.f, 0.f}, up)) + std::abs(glm::dot(glm::vec3 {0.f, 1.f, 0.f}, forward));
		const float newIk = std::abs(glm::dot(glm::vec3 {0.f, 0.f, 1.f}, right)) + std::abs(glm::dot(glm::vec3 {0.f, 0.f, 1.f}, up)) + std::abs(glm::dot(glm::vec3 {0.f, 0.f, 1.f}, forward));
		const aabb globalAABB(globalCenter, newIi, newIj, newIk);

		bool isOnLeft = globalAABB.IsOnOrForwardPlane(camFrustum.leftFace);
		bool isOnRight = globalAABB.IsOnOrForwardPlane(camFrustum.rightFace);
		bool isOnTop = globalAABB.IsOnOrForwardPlane(camFrustum.topFace);
		bool isOnBottom = globalAABB.IsOnOrForwardPlane(camFrustum.bottomFace);
		bool isOnNear = globalAABB.IsOnOrForwardPlane(camFrustum.nearFace);
		bool isOnFar = globalAABB.IsOnOrForwardPlane(camFrustum.farFace);

		return (isOnLeft && isOnRight && isOnTop && isOnBottom && isOnNear && isOnFar);
	};
};

frustum CreateFrustumFromCamera(const class camera* cam);

aabb GenerateAABB(const class model* m);
sphere GenerateSphereBV(const class model* m);

aabb GenerateAABB(const class mesh* m);
sphere GenerateSphereBV(const class mesh* m);
