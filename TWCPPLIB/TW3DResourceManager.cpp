#include "pch.h"
#include "TW3DResourceManager.h"

TW3D::TW3DResourceManager::TW3DResourceManager(TW3DDevice* Device) :
	device(Device)
{
	temp_gcl = new TW3DTempGCL(device);
}

TW3D::TW3DResourceManager::~TW3DResourceManager() {
	delete temp_gcl;
}

TW3D::TW3DResourceVB* TW3D::TW3DResourceManager::CreateVertexBuffer(TWT::UInt VertexCount, TWT::UInt SingleVertexSizeInBytes) {
	return new TW3DResourceVB(device, VertexCount * SingleVertexSizeInBytes, SingleVertexSizeInBytes, temp_gcl);
}

TW3D::TW3DResourceCB* TW3D::TW3DResourceManager::CreateConstantBuffer(TWT::UInt ElementSizeInBytes, TWT::UInt ElementCount) {
	return new TW3DResourceCB(device, ElementSizeInBytes, ElementCount);
}
