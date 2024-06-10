#include "transform.h"

#include "ecs/Coordinator.hpp"

extern Coordinator gCoordinator;
void transform_system::Update()
{
	for (auto entity : mEntities)
	{
		auto& t = gCoordinator.GetComponent<transform>(entity);
		t.ComputeModelMatrix();
	}
}