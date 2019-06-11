#include "pch.h"
#include "Universe3D.h"

Universe3D::Universe3D(TW3DResourceManager* ResourceManager) :
	TW3DObject(ResourceManager, sizeof(TWT::DefaultModelCB))
{
	
}

Universe3D::~Universe3D() = default;

std::vector<UniverseObject> Universe3D::GatherObjects(TWT::double3 Position, TWT::float3 Extent) {
	std::vector<UniverseObject> objects;



	return objects;
}