#include "pch.h"
#include "TW3DModules.h"

static TW3DBitonicSorter* bitonic_sorter;
static TW3DRaytracer* ray_tracer;

void TW3DModules::Initialize(TW3DResourceManager* ResourceManager) {
	bitonic_sorter = new TW3DBitonicSorter(ResourceManager);
	ray_tracer = new TW3DRaytracer(ResourceManager);
}

void TW3DModules::Release() {
	delete bitonic_sorter;
	delete ray_tracer;
}

TW3DBitonicSorter* TW3DModules::BitonicSorter() {
	return bitonic_sorter;
}

TW3DRaytracer* TW3DModules::RayTracer() {
	return ray_tracer;
}