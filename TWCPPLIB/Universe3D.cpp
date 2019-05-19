#include "pch.h"
#include "Universe3D.h"

Universe3D::Universe3D(TW3DResourceManager* ResourceManager) :
	TW3DObject(ResourceManager, sizeof(TWT::DefaultVertexBufferCB))
{
	
}

Universe3D::~Universe3D() {
}

std::vector<UniverseObject> Universe3D::GatherObjects(TWT::vec3d Position, TWT::vec3 Extent) {
	std::vector<UniverseObject> objects;



	return objects;
}