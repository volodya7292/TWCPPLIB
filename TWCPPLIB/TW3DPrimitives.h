#pragma once
#include "TW3DResourceManager.h"

namespace TW3DPrimitives {
	void Initialize(TW3D::TW3DResourceManager* ResourceManager);
	void Cleanup();

	TW3D::TW3DResourceVB* GetCubeVertexBuffer();
}