#pragma once
#include "TW3DBitonicSorter.h"
#include "TW3DRaytracer.h"

namespace TW3DModules {
	void Initialize(TW3DResourceManager* ResourceManager);
	void Release();

	TW3DBitonicSorter* BitonicSorter();
	TW3DRaytracer* RayTracer();
}