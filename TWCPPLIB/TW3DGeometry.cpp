#include "pch.h"
#include "TW3DGeometry.h"

TW3DGeometry::TW3DGeometry(TW3DResourceManager* ResourceManager) :
	TW3DObject(ResourceManager)
{
}

void TW3DGeometry::Update() {
	TW3DObject::Update();
}

std::vector<TW3DVertexMeshInstance>& TW3DGeometry::GetVertexMeshInstances() {
	return VMInstances;
}
