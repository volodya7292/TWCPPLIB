#include "pch.h"
#include "TW3DModules.h"

static TW3DBitonicSorter* bitonic_sorter;
static TW3DVRSCalculator* vrs_calculator;

void TW3DModules::Initialize(TW3DResourceManager* ResourceManager) {
	bitonic_sorter = new TW3DBitonicSorter(ResourceManager);
	vrs_calculator = new TW3DVRSCalculator(ResourceManager);
}

void TW3DModules::Release() {
	delete bitonic_sorter;
	delete vrs_calculator;
}

TW3DBitonicSorter* TW3DModules::BitonicSorter() {
	return bitonic_sorter;
}

TW3DVRSCalculator* TW3DModules::VRSCalculator() {
	return vrs_calculator;
}
