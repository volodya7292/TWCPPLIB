#include "pch.h"
#include "Universe.h"

Universe::Universe(TW3DResourceManager* ResourceManager) :
	TW3DObject(ResourceManager)
{
	
}

Universe::~Universe() = default;

std::vector<UniverseObject> Universe::GatherObjects(TWT::double3 Position, TWT::float3 Extent) {
	std::vector<UniverseObject> objects;



	return objects;
}