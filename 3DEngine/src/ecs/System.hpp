#pragma once

#include "Types.hpp"

#include <set>

class System
{
public:
	std::set<Entity> mEntities;

	virtual void OnEntityAdded(Entity entity) {};
	virtual void OnEntityRemoved(Entity entity) {};
};
