#pragma once
#include "TW3DBitonicSorter.h"

namespace TW3DModules {
	void Initialize(TW3D::TW3DResourceManager* ResourceManager);
	void Release();

	TW3D::TW3DBitonicSorter* BitonicSorter();
}