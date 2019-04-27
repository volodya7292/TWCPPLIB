#include "pch.h"
#include "TW3DModules.h"

static TW3D::TW3DBitonicSorter* bitonic_sorter;

void TW3DModules::Initialize(TW3D::TW3DResourceManager* ResourceManager) {
	bitonic_sorter = new TW3D::TW3DBitonicSorter(ResourceManager);
}

void TW3DModules::Release() {
	delete bitonic_sorter;
}

TW3D::TW3DBitonicSorter* TW3DModules::BitonicSorter() {
	return bitonic_sorter;
}
