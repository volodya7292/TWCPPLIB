#pragma once
#include "TW3DResourceManager.h"
#include "TW3DVertexBuffer.h"

namespace TW3DPrimitives {
	void Initialize(TW3DResourceManager* ResourceManager);;

	TW3DVertexBuffer* GetCubeVertexMesh();
	TW3DVertexBuffer* GetPyramid4VertexMesh();
}