#include "pch.h"
#include "TW3DModules.h"

static TW3DBitonicSorter* bitonic_sorter;
static TW3DBlitter* blitter;

void TW3DModules::Initialize(TW3DResourceManager* ResourceManager) {
	bitonic_sorter = new TW3DBitonicSorter(ResourceManager);
	blitter = new TW3DBlitter(ResourceManager);
}

void TW3DModules::Release() {
	delete bitonic_sorter;
	delete blitter;
}

TW3DBitonicSorter* TW3DModules::BitonicSorter() {
	return bitonic_sorter;
}

TW3DBlitter* TW3DModules::Blitter() {
	return blitter;
}
