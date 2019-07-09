#include "pch.h"
#include "TW3DCube.h"
#include "TW3DPrimitives.h"

TW3DCube::TW3DCube(TW3DResourceManager* ResourceManager) :
	TW3DGeometry(ResourceManager)
{
	VMInstance.VertexBuffer = TW3DPrimitives::GetCubeVertexMesh();
}

void TW3DCube::Update() {
	TW3DGeometry::Update();
}
