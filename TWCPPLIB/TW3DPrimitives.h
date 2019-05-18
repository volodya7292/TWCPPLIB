#pragma once
#include "TW3DResourceManager.h"
#include "TW3DVertexMesh.h"

namespace TW3DPrimitives {
	void Initialize(TW3DResourceManager* ResourceManager);
	void Release();

	TW3DVertexMesh* GetCubeVertexMesh();
	TW3DVertexMesh* GetPyramid4VertexMesh();
}